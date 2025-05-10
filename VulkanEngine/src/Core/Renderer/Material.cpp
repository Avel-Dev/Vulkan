#include "Material.h"
#include "VulkanEngine/VulkanEngine.h"
#include "Renderer/BufferUtils.h"

namespace CHIKU
{
    Material::~Material()
    {
        CleanUp();
    }

    void Material::CreateMaterial(MaterialLayoutPreset preset)
    {
        m_UniformBufferLayout = GetDescriptorLayoutForPreset(preset);
        CreateDescriptorSetLayout();
        CreateDescriptorPool();
        CreateUniformBuffer();
        m_Textures[0].CreateTexture("textures/viking_room.png");
        CreateDescriptorSets();
    }

    void Material::CreateUniformBuffer()
    {
        VkDeviceSize bufferSize = m_UniformBufferLayout.size;
        m_UniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
        m_UniformBuffersMemory.resize(MAX_FRAMES_IN_FLIGHT);
        m_UniformBuffersMapped.resize(MAX_FRAMES_IN_FLIGHT);

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        {
            Utils::CreateBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                m_UniformBuffers[i], m_UniformBuffersMemory[i]);

            vkMapMemory(VulkanEngine::GetDevice(), m_UniformBuffersMemory[i], 0, bufferSize, 0, &m_UniformBuffersMapped[i]);
        }
    }

    void Material::CreateDescriptorPool()
    {
        std::vector<VkDescriptorPoolSize> poolSizes{};
        poolSizes.resize(m_UniformBufferLayout.BufferAttributes.size());

        for (int i = 0;i < m_UniformBufferLayout.BufferAttributes.size(); i++)
        {
            poolSizes[i].type = m_UniformBufferLayout.BufferAttributes[i].DescriptorType;
            poolSizes[i].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
        }

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

    void Material::CreateDescriptorSets()
    {
        std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, m_Layout);
        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = m_DescriptorPool;
        allocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
        allocInfo.pSetLayouts = layouts.data();

        m_DescriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
        if (vkAllocateDescriptorSets(VulkanEngine::GetDevice(), &allocInfo, m_DescriptorSets.data()) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to allocate descriptor sets!");
        }

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) 
        {
            VkDescriptorBufferInfo bufferInfo{};
            bufferInfo.buffer = m_UniformBuffers[i];
            bufferInfo.offset = 0;
            bufferInfo.range = m_UniformBufferLayout.size;

            std::vector<VkDescriptorImageInfo> imageInfo;
            imageInfo.resize(m_Textures.size());
            for (int i = 0; i < m_Textures.size();i++)
            {
                imageInfo[i].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                imageInfo[i].imageView = m_Textures[i].GetTextureImageView();
                imageInfo[i].sampler = m_Textures[i].GetTextureImageSampler();
            }

            std::array<VkWriteDescriptorSet, 2> descriptorWrites{};

            descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[0].dstSet = m_DescriptorSets[i];
            descriptorWrites[0].dstBinding = 0;
            descriptorWrites[0].dstArrayElement = 0;
            descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            descriptorWrites[0].descriptorCount = 1;
            descriptorWrites[0].pBufferInfo = &bufferInfo;

            descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[1].dstSet = m_DescriptorSets[i];
            descriptorWrites[1].dstBinding = 1;
            descriptorWrites[1].dstArrayElement = 0;
            descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            descriptorWrites[1].descriptorCount = 1;
            descriptorWrites[1].pImageInfo = imageInfo.data();

            vkUpdateDescriptorSets(VulkanEngine::GetDevice(), static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
        }

    }

    void Material::Bind(VkCommandBuffer commandBuffer)
    {
    }

    void Material::CleanUp()
    {
        if (m_Layout != VK_NULL_HANDLE)
        {
            vkDestroyDescriptorSetLayout(VulkanEngine::GetDevice(), m_Layout, nullptr);
            vkDestroyDescriptorPool(VulkanEngine::GetDevice(),m_DescriptorPool,nullptr);

            for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
            {
                vkDestroyBuffer(VulkanEngine::GetDevice(), m_UniformBuffers[i], nullptr);
                vkFreeMemory(VulkanEngine::GetDevice(), m_UniformBuffersMemory[i], nullptr);
            }
            for (auto& i : m_Textures)
            {
                i.CleanUp();
            }
        }
    }

    size_t Material::GetAttributeSize(const UniformPlainDataType& attributeType)
    {
        switch (attributeType)
        {
        case UniformPlainDataType::Vec2:
            return sizeof(glm::vec2);  // 2 floats (should be padded to 4 bytes in std140)
        case UniformPlainDataType::Vec3:
            return sizeof(glm::vec3);  // 3 floats (should be padded to 4 floats, or 16 bytes)
        case UniformPlainDataType::Vec4:
            return sizeof(glm::vec4);  // 4 floats
        case UniformPlainDataType::IVec2:
            return sizeof(glm::ivec2); // 2 ints (should be padded to 4 ints, or 16 bytes)
        case UniformPlainDataType::IVec3:
            return sizeof(glm::ivec3); // 3 ints (should be padded to 4 ints, or 16 bytes)
        case UniformPlainDataType::IVec4:
            return sizeof(glm::ivec4); // 4 ints
        case UniformPlainDataType::Mat3:
            return sizeof(glm::mat4);  // 3x3 matrix (should be padded to a 4x4 matrix, 16 floats or 64 bytes)
        case UniformPlainDataType::Mat4:
            return sizeof(glm::mat4);  // 4x4 matrix, already aligned to 16 floats (64 bytes)
        case UniformPlainDataType::Sampler2D:
            return sizeof(VkSampler);  // Typically a pointer, size depends on platform (usually 4 bytes)
        default:
            return 0;  // Undefined or unsupported type
        }
    }

    UniformBufferLayout Material::GetDescriptorLayoutForPreset(const MaterialLayoutPreset& preset)
    {
        UniformBufferLayout layout;

        switch (preset) {
        case MaterialLayoutPreset::None:
            break;

        case MaterialLayoutPreset::MVP:
            layout.BufferAttributes = {
                { "u_MVP", UniformPlainDataType::Mat4, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT }
            };
            break;

        case MaterialLayoutPreset::ModelViewProjection:
            layout.BufferAttributes = {
                { "u_Model", UniformPlainDataType::Mat4, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT },
                { "u_View", UniformPlainDataType::Mat4, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT },
                { "u_Proj", UniformPlainDataType::Mat4, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT }
            };
            break;

        case MaterialLayoutPreset::MVPUnlitTextured:
            layout.BufferAttributes = {
                { "u_MVP", UniformPlainDataType::Mat4, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT },
                { "Texture", UniformOpaqueDataType::Sampler2D, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT }
            };
            break;

        case MaterialLayoutPreset::MVPStandard:
            break;
        }

        layout.size = 0;
        for (auto& b : layout.BufferAttributes)
        {
            if (UniformPlainDataType* plainData = std::get_if<UniformPlainDataType>(&b.AttributeType)) {
                layout.size += GetAttributeSize(*plainData);
            }
        }

        return layout;
    }

    void Material::CreateDescriptorSetLayout()
    {
        std::vector<VkDescriptorSetLayoutBinding> bindings;
        bindings.reserve(m_UniformBufferLayout.BufferAttributes.size());

        int numberOfTextures = 0;
        int bindingLocation = 0;
        for (int i = 0; i < m_UniformBufferLayout.BufferAttributes.size(); i++)
        {
            if (m_UniformBufferLayout.BufferAttributes[i].DescriptorType == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER)
            {
                numberOfTextures++;
            }

            VkDescriptorSetLayoutBinding binding{};
            binding.binding = bindingLocation;
            binding.descriptorType = m_UniformBufferLayout.BufferAttributes[i].DescriptorType;
            binding.descriptorCount = 1;
            binding.stageFlags = m_UniformBufferLayout.BufferAttributes[i].ShaderStageFlag;
            binding.pImmutableSamplers = nullptr;

            bindings.push_back(binding);

            if ((i + 1) != m_UniformBufferLayout.BufferAttributes.size() && std::holds_alternative<UniformOpaqueDataType>(m_UniformBufferLayout.BufferAttributes[i + 1].AttributeType))
            {
                bindingLocation++;
            }
        }

        m_Textures.resize(numberOfTextures);
        VkDescriptorSetLayoutCreateInfo layoutInfo{};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
        layoutInfo.pBindings = bindings.data();

        if (vkCreateDescriptorSetLayout(VulkanEngine::GetDevice(), &layoutInfo, nullptr, &m_Layout) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create descriptor set layout!");
        }
    }
}