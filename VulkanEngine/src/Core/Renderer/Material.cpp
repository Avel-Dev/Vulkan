#include "Material.h"
#include "VulkanEngine/VulkanEngine.h"
#include "Utils/BufferUtils.h"
#include "VulkanEngine/Window.h"
#include "Utils/ImageUtils.h"

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/hash.hpp>

#include <chrono>

namespace CHIKU
{
	std::string Material::GetMaterialShader(MaterialPresets presets)
	{
		ZoneScoped;

		switch (presets)
		{
		case CHIKU::Lit: return "default/lit";
		case CHIKU::Unlit: return "default/unlit";
		}

		return "default/unlit";
	}

	void Material::CreateMaterial(MaterialPresets presets, const char* textures[], int size)
	{
		ZoneScoped;

		m_Preset = presets;
		m_ShaderID = GetMaterialShader(m_Preset);
	}

	void Material::Bind(VkPipelineLayout pipelineLayout) const
	{
		ZoneScoped;
		//vkCmdBindDescriptorSets(VulkanEngine::GetCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &m_UniformBuffer.DescriptorSets[VulkanEngine::GetCurrentFrame()], 0, nullptr);
	}
}