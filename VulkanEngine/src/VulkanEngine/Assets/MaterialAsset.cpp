#include "MaterialAsset.h"
#include "AssetManager.h"
#include "Utils/ShaderUtils.h"
#include "Utils/BufferUtils.h"

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

        for (const auto& [setIndex, set] : bufferDescription)
        {
            for (const auto& [bindingIndex, uniformBuffer] : set) 
            {
                if (!uniformBuffer.isUBO())
                    continue;

                UniformBufferStorage storage;
                storage.DataBuffer.resize(uniformBuffer.Size);

                for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) 
                {
                     Utils::CreateBuffer(uniformBuffer.Size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, storage.UniformBuffers[i], storage.UniformBuffersMemory[i]);

                    vkMapMemory(VulkanEngine::GetDevice(), storage.UniformBuffersMemory[i], 0, uniformBuffer.Size, 0, &storage.UniformBuffersMapped[i]);
                }

                m_UniformSetStorage[setIndex].BindingStorage[bindingIndex] = storage;
            }
        }

        CreateDescriptorPool();
        CreateDescriptorSetLayout(bufferDescription);
        CreateDescriptorSets();
    }

    void MaterialAsset::CreateDescriptorPool()
    {
        ZoneScoped;

        std::array<VkDescriptorPoolSize, 2> poolSizes{};
        poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        poolSizes[0].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
        poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        poolSizes[1].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

        VkDescriptorPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
        poolInfo.pPoolSizes = poolSizes.data();
        poolInfo.maxSets = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

        if (vkCreateDescriptorPool(VulkanEngine::GetDevice(), &poolInfo, nullptr, &m_DescriptorPool) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create descriptor pool!");
        }
    }

    void MaterialAsset::CreateDescriptorSetLayout(const UniformBufferDescription& bufferDescription)
    {
        ZoneScoped;

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

            m_UniformSetStorage[setIndex].DescriptorSetLayout = layout;
		}
    }

    void MaterialAsset::CreateDescriptorSets()
    {
        ZoneScoped;

        for (auto& [setIndex, layout] : m_UniformSetStorage)
        {
            std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, layout.DescriptorSetLayout);

            VkDescriptorSetAllocateInfo allocInfo{};
            allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
            allocInfo.descriptorPool = m_DescriptorPool;
            allocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
            allocInfo.pSetLayouts = layouts.data();

            if (vkAllocateDescriptorSets(VulkanEngine::GetDevice(), &allocInfo, layout.DescriptorSets.data()) != VK_SUCCESS)
            {
                throw std::runtime_error("failed to allocate descriptor sets!");
            }

            const auto& bufferDescription = m_Shader->GetBufferDescription();

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

		const auto& bufferDescription = m_Shader->GetBufferDescription();

        for (auto& [setIndex, set] : m_UniformSetStorage)
        {
            for (auto& [bindingIndex, storage] : set.BindingStorage)
            {
		//		//const auto& uniformBuffer = bufferDescription.find(setIndex)->second.find(bindingIndex)->second;
  //  //            if (!uniformBuffer.isValid() || uniformBuffer.isSampler())
  //  //            {
  //  //                throw std::runtime_error("It is not a valid uniform buffer");
  //  //            }

  //  //            // Replace this with actual material data
  //  //            memcpy(storage.UniformBuffersMapped[currentFrame],&currentFrame, uniformBuffer.Size);
            }
        }

        auto& let = m_UniformSetStorage[0].BindingStorage[0].UniformBuffersMapped[currentFrame];

        static auto startTime = std::chrono::high_resolution_clock::now();

        auto currentTime = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

        glm::mat4 model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        glm::mat4 view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float)Window::WIDTH / (float)Window::HEIGHT, 0.1f, 10.0f);

        proj[1][1] *= -1;

        glm::mat4 data[3] = { model,view,proj };

        memcpy(let, data, sizeof(glm::mat4) * 3);
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

        vkDestroyDescriptorPool(VulkanEngine::GetDevice(), m_DescriptorPool, nullptr);

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

    void MaterialAsset::Bind(VkPipelineLayout pipelineLayout)
    {
        ZoneScoped;
        UpdateUniformBuffer(VulkanEngine::GetCurrentFrame());
        
        for (const auto& [setIndex, uniformStorage] : m_UniformSetStorage)
        {
            vkCmdBindDescriptorSets(VulkanEngine::GetCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &uniformStorage.DescriptorSets[VulkanEngine::GetCurrentFrame()], 0, nullptr);
        }
    }
}