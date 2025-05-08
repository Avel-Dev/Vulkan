#include "Material.h"
#include "VulkanEngine/VulkanEngine.h"

namespace CHIKU
{
    DescriptorLayout GetDescriptorLayoutForPreset(MaterialLayoutPreset preset) 
    {
        DescriptorLayout layout;

        switch (preset) {
        case MaterialLayoutPreset::None:
            break;

        case MaterialLayoutPreset::ModelOnly:
            layout.push_back({ 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT });
            break;

        case MaterialLayoutPreset::MVP:
            layout.push_back({ 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT });
            break;

        case MaterialLayoutPreset::ModelViewProjection:
            layout.push_back({ 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT });
            break;

        case MaterialLayoutPreset::UnlitTextured:
            layout.push_back({ 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT });
            layout.push_back({ 1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT });
            break;

        case MaterialLayoutPreset::PBR_Standard:
            layout.push_back({ 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT });
            layout.push_back({ 1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT }); // Albedo
            layout.push_back({ 2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT }); // Metallic
            layout.push_back({ 3, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT }); // Roughness
            layout.push_back({ 4, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT }); // Normal
            break;

        case MaterialLayoutPreset::Skybox:
            layout.push_back({ 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT });
            layout.push_back({ 1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT }); // Cube Map
            break;

        case MaterialLayoutPreset::PostProcess:
            layout.push_back({ 0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT }); // Screen texture
            break;

        case MaterialLayoutPreset::Custom:
            // This should be user-defined; leave empty or route to custom handler
            break;
        }

        return layout;
    }

    VkDescriptorSetLayout CreateDescriptorSetLayoutFromPreset(VkDevice device, MaterialLayoutPreset preset) 
    {
        if (preset == MaterialLayoutPreset::None)
        {
            return VK_NULL_HANDLE;
        }

        DescriptorLayout layout = GetDescriptorLayoutForPreset(preset);

        std::vector<VkDescriptorSetLayoutBinding> bindings;
        bindings.reserve(layout.size());

        for (const auto& b : layout) 
        {
            VkDescriptorSetLayoutBinding binding{};
            binding.binding = b.binding;
            binding.descriptorType = b.type;
            binding.descriptorCount = 1;
            binding.stageFlags = b.stageFlags;
            binding.pImmutableSamplers = nullptr;

            bindings.push_back(binding);
        }

        VkDescriptorSetLayoutCreateInfo layoutInfo{};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
        layoutInfo.pBindings = bindings.data();

        VkDescriptorSetLayout descriptorSetLayout;
        if (vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create descriptor set layout!");
        }

        return descriptorSetLayout;
    }

    void Material::SetLayout(MaterialLayoutPreset preset)
    {
        m_Layout = CreateDescriptorSetLayoutFromPreset(VulkanEngine::GetDevice(), preset);
    }
}