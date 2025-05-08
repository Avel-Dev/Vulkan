#pragma once
#include "VulkanHeader.h"

namespace CHIKU
{
    enum class MaterialLayoutPreset
    {
        None = 0,

        // Only model matrix (for basic unlit or debug materials)
        ModelOnly,

        // Model + ViewProjection matrix (common for static simple shaders)
        MVP,

        // Model + View + Projection as separate (more flexible binding)
        ModelViewProjection,

        // PBR Material with albedo, metallic, roughness, normal map
        PBR_Standard,

        // Unlit textured material
        UnlitTextured,

        // Skybox (usually only ViewProjection)
        Skybox,

        // Post-processing effect (fullscreen, usually no model matrix)
        PostProcess,

        // Custom user-defined layout
        Custom
    };

    struct DescriptorBinding 
    {
        uint32_t binding;
        VkDescriptorType type;
        VkShaderStageFlags stageFlags;
    };

    using DescriptorLayout = std::vector<DescriptorBinding>;

    DescriptorLayout GetDescriptorLayoutForPreset(MaterialLayoutPreset preset);
    VkDescriptorSetLayout CreateDescriptorSetLayoutFromPreset(VkDevice device, MaterialLayoutPreset preset);

	class Material
	{
    public:
        void SetLayout(MaterialLayoutPreset preset);

    private:
        VkDescriptorSetLayout m_Layout;
	};
}