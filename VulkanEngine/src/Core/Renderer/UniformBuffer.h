#pragma once
#include "VulkanHeader.h"
#include <variant>
#include "UniformDescription.h"

namespace CHIKU
{
    class UniformBuffer
    {
    public:
        static void Init(); //Create Descriptor Pool and Generic Descriptor Set layout and Descriptor Sets.
        static void Bind(VkPipelineLayout pipelineLayout); //Bind the Generic Descriptor Sets.
        static VkDescriptorSetLayout GetDescriptorSetLayout(GenericUniformBuffers presets) { return sm_BufferDescriptions[presets].DescriptorSetLayouts; }
        static void Update();
        static void CleanUp();

    private:
        static void CreateDescriptorPool();

        static UniformBufferDescription GetOrBuildUniform(GenericUniformBuffers presets);
        static UniformBufferDescription CreateUniformDescription(GenericUniformBuffers presets);
        static VkDescriptorSetLayout CreateDescriptorSetLayout(const UniformBufferLayout& presets);
        static void CreateDescriptorSets(const UniformBufferLayout& layout,
            VkDescriptorSetLayout descriptorSetLayouts, 
            std::array<VkDescriptorSet, MAX_FRAMES_IN_FLIGHT>& descriptorSets,
            std::array<VkBuffer, MAX_FRAMES_IN_FLIGHT>& uniformBuffers,
            /* Remove this */
            TextureData texture);

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