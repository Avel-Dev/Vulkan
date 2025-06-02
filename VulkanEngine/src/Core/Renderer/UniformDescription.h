#pragma once
#include "VulkanHeader.h"

namespace CHIKU
{
    enum class GenericUniformBuffers : uint8_t
    {
        MVP,
        Color
    };

    enum class UniformPlainDataType : uint8_t
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

    enum class UniformOpaqueDataType : uint8_t
    {
        DIFFUSE_TEXTURE,
		NORMAL_TEXTURE,
		SPECULAR_TEXTURE,
		/*EMISSIVE_TEXTURE,
		HEIGHT_TEXTURE,
		SHADOW_MAP_TEXTURE,
		SHADOW_CUBE_MAP_TEXTURE,
		SHADOW_MAP_ARRAY_TEXTURE,
		SHADOW_CUBE_MAP_ARRAY_TEXTURE,
		SHADOW_MAP_ARRAY_TEXTURE_2D,
		SHADOW_CUBE_MAP_ARRAY_TEXTURE_2D,
		SHADOW_MAP_ARRAY_TEXTURE_3D,
		SHADOW_CUBE_MAP_ARRAY_TEXTURE_3D,
		SHADOW_MAP_ARRAY_TEXTURE_2D_ARRAY,
		SHADOW_CUBE_MAP_ARRAY_TEXTURE_2D_ARRAY,
		SHADOW_MAP_ARRAY_TEXTURE_3D_ARRAY,
		SHADOW_CUBE_MAP_ARRAY_TEXTURE_3D_ARRAY,*/
        none
    };

    struct UniformPlainData
    {
        std::vector<UniformPlainDataType> Types;
        uint32_t Size = 0;   // Size in bytes of this data type
		uint32_t Binding = 0; // Binding point for this uniform data
    };

    struct UniformOpaqueData
    {
        UniformOpaqueDataType Type = UniformOpaqueDataType::none;
        uint32_t Binding = 0; // Binding point for this uniform data

        // Overload operator bool
        operator bool() const 
        {
            return Type != UniformOpaqueDataType::none;  // or any other logic that makes sense
        }
    };

    struct UniformBufferLayout
    {
        UniformPlainData PlainAttributes;
        UniformOpaqueData OpeaquData;
    };

    struct TextureData
    {
        VkImage TextureImage;
        VkImageView TextureImageView;
        VkDeviceMemory TextureImageMemory;
        VkSampler TextureSampler;
    };

    struct UniformBufferDescription
    {
        VkDescriptorSetLayout DescriptorSetLayouts;
        std::array<VkBuffer, MAX_FRAMES_IN_FLIGHT> UniformBuffers;
        std::array<VkDeviceMemory, MAX_FRAMES_IN_FLIGHT> UniformBuffersMemory;
        std::array<void*, MAX_FRAMES_IN_FLIGHT> UniformBuffersMapped;
        std::array<VkDescriptorSet, MAX_FRAMES_IN_FLIGHT> DescriptorSets;
    };
}