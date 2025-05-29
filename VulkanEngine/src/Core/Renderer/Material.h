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

		struct BufferData
		{
			GenericUniformBuffers BufferType;
			std::array<VkBuffer, MAX_FRAMES_IN_FLIGHT> UniformBuffers;
			std::array<VkDeviceMemory, MAX_FRAMES_IN_FLIGHT> UniformBuffersMemory;
			std::array<void*, MAX_FRAMES_IN_FLIGHT> UniformBuffersMapped;
			std::array<VkDescriptorSet, MAX_FRAMES_IN_FLIGHT> DescriptorSets;
		};

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