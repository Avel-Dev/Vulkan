#pragma once
#include "VulkanHeader.h"
#include <variant>

namespace CHIKU
{
    class UniformBuffer
    {
    public:
        enum class GenericUniformBuffers
        {
            VP
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
            VkDescriptorType DescriptorType;
            VkShaderStageFlags ShaderStageFlag;
        };

        struct UniformBufferLayout
        {
            std::vector<PlainUniformBufferAttribute> PlainBufferAttributes;
            uint32_t Binding;
            size_t Size;
        };

    public:
        static void Init(); //Create Descriptor Pool and Generic Descriptor Set layout and Descriptor Sets.
        static void Bind(VkPipelineLayout pipelineLayout); //Bind the Generic Descriptor Sets.
        static VkDescriptorSetLayout GetDescriptorSetLayout(GenericUniformBuffers presets) { return sm_BufferDescriptions[presets].DescriptorSetLayouts; }
        static void Update();
        static void CleanUp();

    private:
        struct UniformBufferDescription
        {
            UniformBufferLayout UniformBufferLayouts;
            VkDescriptorSetLayout DescriptorSetLayouts;
            std::array<VkBuffer, MAX_FRAMES_IN_FLIGHT> UniformBuffers;
            std::array<VkDeviceMemory, MAX_FRAMES_IN_FLIGHT> UniformBuffersMemory;
            std::array<void*, MAX_FRAMES_IN_FLIGHT> UniformBuffersMapped;
            std::array<VkDescriptorSet, MAX_FRAMES_IN_FLIGHT> DescriptorSets;
        };

        static void CreateDescriptorPool();

        static UniformBufferDescription GetOrBuildUniform(GenericUniformBuffers presets);
        static UniformBufferDescription CreateUniformDescription(GenericUniformBuffers presets);
        static VkDescriptorSetLayout CreateDescriptorSetLayout(GenericUniformBuffers presets);
        static void CreateDescriptorSets(const UniformBufferLayout& layout,
            VkDescriptorSetLayout descriptorSetLayouts, 
            std::array<VkDescriptorSet, MAX_FRAMES_IN_FLIGHT>& descriptorSets,
            std::array<VkBuffer, MAX_FRAMES_IN_FLIGHT>& uniformBuffers);

        static void CreateUniformBuffer(size_t Size, 
            std::array<VkBuffer, MAX_FRAMES_IN_FLIGHT>& UniformBuffers, 
            std::array<VkDeviceMemory, MAX_FRAMES_IN_FLIGHT>& UniformBuffersMemory,
            std::array<void*, MAX_FRAMES_IN_FLIGHT>& UniformBuffersMapped);

        static void FinalizeLayout(UniformBufferLayout& layout);
        static UniformBufferLayout GetUniformBufferLayout(GenericUniformBuffers BufferType);

        static VkDescriptorPool sm_DescriptorPool;
        static std::unordered_map<GenericUniformBuffers, UniformBufferDescription> sm_BufferDescriptions;
    };
}