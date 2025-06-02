#pragma once
#include "UniformBuffer.h"
#include "VulkanHeader.h"
#include "VertexBuffer.h"
#include "Shader.h"

namespace CHIKU
{
	enum MaterialPresets
	{
		Lit,
		Unlit,
	};

	class Material
	{
	public:
		void CreateMaterial(MaterialPresets presets, const char* textures[] = {},int size = 0);

		inline MaterialPresets GetMaterialType() const { return m_Preset; }
		inline std::string GetShaderID() const { return m_ShaderID; }

		void Bind(VkPipelineLayout pipelineLayout) const;
		void CleanUp() {}

		const inline VkDescriptorSetLayout GetLayout() const { return m_UniformBuffer.DescriptorSetLayouts; };

	private:
		std::string GetMaterialShader(MaterialPresets presets);

	private:
		UniformBufferDescription m_UniformBuffer;
		MaterialPresets m_Preset;

		std::string m_ShaderID;
	};

}