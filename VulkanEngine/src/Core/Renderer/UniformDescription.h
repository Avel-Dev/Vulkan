#pragma once
#include "VulkanHeader.h"

namespace CHIKU
{
    enum class GenericUniformBuffers
    {
        MVP
    };

    enum class UniformPlainDataType
    {
        Vec2,           // 2 x 32-bit float (e.g., UVs)
        Vec3,           // 3 x 32-bit float (e.g., position, normal)
        Vec4,           // 4 x 32-bit float (e.g., color, tangent, weights)

        IVec2,          // 2 x int
        IVec3,          // 3 x int
        IVec4,          // 4 x int (e.g., bone indices)

        Mat3,
        Mat4,
        Sampler2D
    };

    enum class UniformOpaqueDataType
    {
        Sampler2D,
        Sampler3D,
        SamplerCube
    };

    struct PlainUniformBufferAttribute
    {
        std::string AttributeName;
        UniformPlainDataType AttributeType;
        VkShaderStageFlags ShaderStageFlag;
    };

    struct OpaqueUniformBufferAttribute
    {
        std::string AttributeName;
        UniformOpaqueDataType AttributeType;
        VkShaderStageFlags ShaderStageFlag;
    };

    struct UniformBufferLayout
    {
        std::vector<PlainUniformBufferAttribute> PlainBufferAttributes;
        std::vector<OpaqueUniformBufferAttribute> OpaqueBufferAttributes;
        uint32_t Binding;
        size_t Size;
    };

    struct TextureData
    {
        VkImage TextureImage;
        VkDeviceMemory TextureImageMemory;
        VkImageView textureImageView;
        VkSampler textureSampler;
    };

    struct UniformBufferDescription
    {
        UniformBufferLayout UniformBufferLayouts;
        VkDescriptorSetLayout DescriptorSetLayouts;
        std::array<VkBuffer, MAX_FRAMES_IN_FLIGHT> UniformBuffers;
        std::array<VkDeviceMemory, MAX_FRAMES_IN_FLIGHT> UniformBuffersMemory;
        std::array<void*, MAX_FRAMES_IN_FLIGHT> UniformBuffersMapped;
        std::array<VkDescriptorSet, MAX_FRAMES_IN_FLIGHT> DescriptorSets;
        TextureData Texture;
    };
}