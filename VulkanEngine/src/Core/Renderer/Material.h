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

	struct MaterialTextuers
	{
		UniformOpaqueDataType dataType;
		std::string path;
	};

	class Material
	{
	public:
		void CreateMaterial(MaterialPresets presets);
		void Bind(VkPipelineLayout pipelineLayout) const;
		void CleanUp();

		inline MaterialPresets GetMaterialType() const { return m_Preset; }
		inline std::string GetShaderID() const { return m_ShaderID; }
		const inline VkDescriptorSetLayout GetLayout() const { return m_UniformBufferDescription.DescriptorSetLayouts; };

		void CreateTexture(MaterialTextuers textures[MAX_TEXTURE_PER_MATERIAL],uint8_t size);

	private:
		std::string GetMaterialShader(MaterialPresets presets);

	private:
		UniformBufferDescription m_UniformBufferDescription;
		MaterialPresets m_Preset;

		TextureData m_Textures[MAX_TEXTURE_PER_MATERIAL];
		std::string m_ShaderID;
	};

}