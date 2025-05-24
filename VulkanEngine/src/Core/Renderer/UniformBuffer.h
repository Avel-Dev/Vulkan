#pragma once
#include "VulkanHeader.h"
#include <variant>

namespace CHIKU
{
    class UniformBuffer
    {

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

    struct UniformBufferAttribute
    {
        std::string AttributeName;
        std::variant<UniformPlainDataType, UniformOpaqueDataType> AttributeType;
        VkDescriptorType DescriptorType;
        VkShaderStageFlags ShaderStageFlag;

        UniformBufferAttribute(const std::string& name, UniformPlainDataType plain, VkDescriptorType descriptorType, VkShaderStageFlags stage)
            : AttributeName(name), AttributeType(plain), DescriptorType(descriptorType), ShaderStageFlag(stage)
        {
        }

        UniformBufferAttribute(const std::string& name, UniformOpaqueDataType opaque, VkDescriptorType descriptorType, VkShaderStageFlags stage)
            : AttributeName(name), AttributeType(opaque), DescriptorType(descriptorType), ShaderStageFlag(stage)
        {
        }
    };

    struct UniformBufferLayout
    {
        std::vector<UniformBufferAttribute> BufferAttributes;
        uint32_t Binding;
    };

    };
}