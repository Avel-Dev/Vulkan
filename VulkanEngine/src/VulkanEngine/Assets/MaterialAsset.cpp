#include "MaterialAsset.h"
#include "AssetManager.h"
#include "Utils/ShaderUtils.h"
#include "Utils/BufferUtils.h"
#include "VulkanEngine/DescriptorPool.h"    

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/hash.hpp>

#include <chrono>
#include <json.hpp>
#include <fstream>

namespace CHIKU
{
    Material MaterialAsset::LoadMaterialFromFile(const AssetPath& filePath)
    {
        ZoneScoped;

        std::string filePathStr = SOURCE_DIR + filePath;
        std::ifstream file(filePathStr);
        if (!file)
        {
            throw std::runtime_error("Failed to open file: " + filePath);
        }

        nlohmann::json j;
        file >> j;

        Material mat;
        mat.name = j["name"];
        mat.shader = j["shader"];

        const auto& cfg = j["Config"];
        mat.config.depthTest = cfg["depthTest"];
        mat.config.depthWrite = cfg["depthWrite"];
        mat.config.blendEnabled = cfg["blendEnabled"];
        mat.config.cullMode = cfg["cullMode"];
        mat.config.frontFace = cfg["frontFace"];
        mat.config.polygonMode = cfg["polygonMode"];
        mat.config.topology = cfg["topology"];

        float r = j["baseColorFactor"][0];
        float g = j["baseColorFactor"][1];
        float b = j["baseColorFactor"][2];
        float w = j["baseColorFactor"][3];

		mat.config.baseColor = glm::vec4(r,g,b,w);

        return mat;
    }

	void MaterialAsset::CreateMaterial()
	{
        ZoneScoped;

        m_Material = LoadMaterialFromFile(m_SourcePath);
        auto shaderAssetHandle = AssetManager::GetShaderAssetHandle(m_Material.shader);

        if (shaderAssetHandle == Asset::InvalidHandle)
        {
            throw std::runtime_error("Invalid Shader Asset Handle that means shader does not exist");
        }

		m_Shader = std::dynamic_pointer_cast<ShaderAsset>(AssetManager::GetAsset(shaderAssetHandle));

        if (!m_Shader)
        {
            throw std::runtime_error("Shader asset not found: " + m_Material.shader);
		}

		CreateUniformBuffer();
	}

    void MaterialAsset::CreateUniformBuffer()
    {
        ZoneScoped;

        const auto& bufferDescription = m_Shader->GetBufferDescription();
        m_UniformSetStorage = CreateDescriptorSetLayout(bufferDescription);
        CreateDescriptorSets( bufferDescription, m_UniformSetStorage);

        if(m_UniformSetStorage.empty())
            return;

        for (int frameNumber = 0; frameNumber < MAX_FRAMES_IN_FLIGHT; frameNumber++)
        {
            m_DescriptorSetsChache[frameNumber].resize(m_UniformSetStorage.size());

            int i = 0;
            for (const auto& [setIndex, storage] : m_UniformSetStorage)
            {
                m_DescriptorSetsChache[frameNumber][i] = storage.DescriptorSets[frameNumber];
                i++;
            }
        }

    }

    std::map<uint32_t, UniformSetStorage>  MaterialAsset::CreateDescriptorSetLayout(const UniformBufferDescription& bufferDescription)
    {
        ZoneScoped;
        std::map<uint32_t, UniformSetStorage> setStorage;

        for (const auto& [setIndex, set] : bufferDescription)
        {
            for (const auto& [bindingIndex, uniformBuffer] : set)
            {
                if (!uniformBuffer.isUBO())
                    continue;

                UniformBufferStorage storage;

                for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
                {
                    Utils::CreateBuffer(uniformBuffer.Size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, storage.UniformBuffers[i], storage.UniformBuffersMemory[i]);

                    vkMapMemory(VulkanEngine::GetDevice(), storage.UniformBuffersMemory[i], 0, uniformBuffer.Size, 0, &storage.UniformBuffersMapped[i]);
                }

                setStorage[setIndex].BindingStorage[bindingIndex] = storage;
            }
        }

        for(const auto& [setIndex,set] : bufferDescription)
        {
			std::vector<VkDescriptorSetLayoutBinding> bindings;
            for(const auto& [bindingIndex,uniformBuffer] : set)
            {
                if (!uniformBuffer.isValid())
                    continue;

                VkDescriptorSetLayoutBinding layoutBinding{};
                layoutBinding.binding = bindingIndex;
                layoutBinding.descriptorCount = 1; // or uniformBuffer.SamplerInfo.Count
                layoutBinding.pImmutableSamplers = nullptr;
                layoutBinding.stageFlags = Utils::MapToVulkanShaderStageFlags(uniformBuffer.Stages);

                if (uniformBuffer.isUBO())
                    layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                else if (uniformBuffer.isSampler())
                    layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                else
                    continue;

                bindings.push_back(layoutBinding);
			}

            if (bindings.empty())
                continue;

            VkDescriptorSetLayoutCreateInfo layoutInfo{};
            layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
            layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
            layoutInfo.pBindings = bindings.data();

            VkDescriptorSetLayout layout;
            if (vkCreateDescriptorSetLayout(VulkanEngine::GetDevice(), &layoutInfo, nullptr, &layout) != VK_SUCCESS) {
                throw std::runtime_error("failed to create descriptor set layout!");
            }

            setStorage[setIndex].DescriptorSetLayout = layout;
		}

		return setStorage;
    }

    void MaterialAsset::CreateDescriptorSets(const UniformBufferDescription& bufferDescription, std::map<uint32_t, UniformSetStorage>& setStorage)
    {
        ZoneScoped;

        for (auto& [setIndex, layout] : setStorage)
        {
            std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, layout.DescriptorSetLayout);

            VkDescriptorSetAllocateInfo allocInfo{};
            allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
            allocInfo.descriptorPool = DescriptorPool::GetDescriptorPool();
            allocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
            allocInfo.pSetLayouts = layouts.data();

            if (vkAllocateDescriptorSets(VulkanEngine::GetDevice(), &allocInfo, layout.DescriptorSets.data()) != VK_SUCCESS)
            {
                throw std::runtime_error("failed to allocate descriptor sets!");
            }

            for (const auto& [bindingIndex, uniformBuffer] : bufferDescription.at(setIndex))
            {
                if (!uniformBuffer.isValid())
                    continue;

                VkWriteDescriptorSet descriptorWrites[MAX_FRAMES_IN_FLIGHT]{};
                VkDescriptorBufferInfo bufferInfos[MAX_FRAMES_IN_FLIGHT]{};
                VkDescriptorImageInfo imageInfos[MAX_FRAMES_IN_FLIGHT]{};

                for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
                {
                    VkWriteDescriptorSet descriptorWrite{};
                    descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                    descriptorWrite.dstSet = layout.DescriptorSets[i];
                    descriptorWrite.dstBinding = bindingIndex;
                    descriptorWrite.dstArrayElement = 0;
                    descriptorWrite.descriptorCount = 1;

                    if (uniformBuffer.isUBO())
                    {
                        auto& bufferStorage = layout.BindingStorage.at(bindingIndex);
                        bufferInfos[i] = {
                            bufferStorage.UniformBuffers[i],
                            0,
                            uniformBuffer.Size
                        };

                        descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                        descriptorWrite.pBufferInfo = &bufferInfos[i];
                    }
                    else if (uniformBuffer.isSampler())
                    {
                        throw std::runtime_error("textures are not implemented yet");
                        //TODO :: ADD Textures

                       /* imageInfos[i] = {
                            textureSampler,
                            textureImageView,
                            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
                            };

                        descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                        descriptorWrite.pImageInfo = &imageInfos[i];*/
                    }

                    descriptorWrites[i] = descriptorWrite;
                }

                vkUpdateDescriptorSets(VulkanEngine::GetDevice(), MAX_FRAMES_IN_FLIGHT, descriptorWrites, 0, nullptr);
            }
        }
    }

    void MaterialAsset::UpdateUniformBuffer(uint32_t currentFrame)
    {
        ZoneScoped;

        if (m_UniformSetStorage.find(1) == m_UniformSetStorage.end())
        {
			throw std::runtime_error("Uniform Set 1 not found, it should contain base color");
        }

        auto& color = m_UniformSetStorage[1].BindingStorage[0].UniformBuffersMapped[currentFrame];
        memcpy(color, &m_Material.config.baseColor, sizeof(glm::vec4));
    }

    void MaterialAsset::CleanUp()
    {
        ZoneScoped;

        Asset::CleanUp();

        for (auto& [set, storage] : m_UniformSetStorage)
        {
            vkDestroyDescriptorSetLayout(VulkanEngine::GetDevice(), storage.DescriptorSetLayout, nullptr);

            for (auto& [bindingIndex, bindingStorage] : storage.BindingStorage)
            {
                for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
                {
                    if (bindingStorage.UniformBuffersMapped[i])
                    {
                        vkUnmapMemory(VulkanEngine::GetDevice(), bindingStorage.UniformBuffersMemory[i]);
                        bindingStorage.UniformBuffersMapped[i] = nullptr;
                    }

                    vkDestroyBuffer(VulkanEngine::GetDevice(), bindingStorage.UniformBuffers[i], nullptr);
                    vkFreeMemory(VulkanEngine::GetDevice(), bindingStorage.UniformBuffersMemory[i], nullptr);
                }
            }
        }

        m_UniformSetStorage.clear();
    }

    MaterialAsset::~MaterialAsset()
    {
        ZoneScoped;

        CleanUp();
    }

    std::vector<VkDescriptorSetLayout> MaterialAsset::GetDescriptorSetLayouts() const
    {
        ZoneScoped;

		std::vector<VkDescriptorSetLayout> layouts;

        for(const auto& [setIndex, storage] : m_UniformSetStorage)
        {
            layouts.push_back(storage.DescriptorSetLayout);
		}

        return layouts;
    }
}