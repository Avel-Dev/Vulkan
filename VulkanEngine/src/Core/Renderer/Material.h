#pragma once
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

		void CreateMaterial(MaterialPresets presets);
		inline MaterialPresets GetMaterialType() const { return m_Preset; }
		inline std::string GetShaderID() const { return m_ShaderID; }

		void Bind(VkPipelineLayout pipelineLayout) const;
		void CleanUp() {}

	private:
		std::string GetMaterialShader(MaterialPresets presets);

	private:

		MaterialPresets m_Preset;
		std::string m_ShaderID;
	};

}