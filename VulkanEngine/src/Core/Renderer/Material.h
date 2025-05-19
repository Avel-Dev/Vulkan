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
		struct UniformBufferObject
		{
			glm::mat4 model;
			glm::mat4 view;
			glm::mat4 proj;
		};

		void CreateMaterial(MaterialPresets presets);
		inline VkDescriptorSetLayout GetDescriptorLayout() const { return GetOrBuildDescriptorLayout(m_Preset); }
		inline MaterialPresets GetMaterialType() const { return m_Preset; }
		inline ShaderID GetShaderID() const { return m_ShaderID; }

		void Update();
		void Bind(VkPipelineLayout pipelineLayout);
		void CleanUp();
		static void StaticCleanUp();

	private:
		static VkDescriptorSetLayout GetOrBuildDescriptorLayout(MaterialPresets presets);
		static VkDescriptorSetLayout CreateDescriptorSetLayout(MaterialPresets presets);
		static VkDescriptorPool CreateDescriptorPool(MaterialPresets presets);

		ShaderID GetMaterialShader(MaterialPresets presets);

		void CreateDescriptorSets();
		void CreateUniformBuffer();
	private:

		MaterialPresets m_Preset;
		ShaderID m_ShaderID;

		static std::map<MaterialPresets, VkDescriptorSetLayout> m_DescriptorSetLayouts;
		static std::map<MaterialPresets, VkDescriptorPool> m_DescriptorPools;

		std::vector<VkBuffer> m_UniformBuffers;
		std::vector<VkDeviceMemory> m_UniformBuffersMemory;
		std::vector<void*> m_UniformBuffersMapped;
		std::vector<VkDescriptorSet> m_DescriptorSets;

		VkDescriptorSetLayout m_MaterialLayout;
		VkDescriptorPool m_MaterialMemoryPool;
	};

}