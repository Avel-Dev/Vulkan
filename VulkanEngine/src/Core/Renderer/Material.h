#pragma once
#include "VulkanHeader.h"
#include "VertexBuffer.h"
#include "Shader.h"
#include <variant>

namespace CHIKU
{
	enum MaterialPresets
	{
		Lit,
		Unlit,
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


	struct UniformBufferAttribute
	{
		std::string AttributeName;
		std::variant<UniformPlainDataType, UniformOpaqueDataType> AttributeType;
		VkDescriptorType DescriptorType;
		VkShaderStageFlags ShaderStageFlag;

		UniformBufferAttribute(const std::string& name, UniformPlainDataType plain, VkDescriptorType descriptorType, VkShaderStageFlags stage)
			: AttributeName(name), AttributeType(plain), DescriptorType(descriptorType), ShaderStageFlag(stage) {
		}

		UniformBufferAttribute(const std::string& name, UniformOpaqueDataType opaque, VkDescriptorType descriptorType, VkShaderStageFlags stage)
			: AttributeName(name), AttributeType(opaque), DescriptorType(descriptorType), ShaderStageFlag(stage) {
		}
	};

	struct UniformBufferLayout
	{
		std::vector<UniformBufferAttribute> BufferAttributes;
		size_t size;
	};


	class Material
	{
	public:

		void CreateMaterial(MaterialPresets presets);
		inline VkDescriptorSetLayout GetDescriptorLayout() const { return GetOrBuildDescriptorLayout(m_Preset); }
		inline MaterialPresets GetMaterialType() const { return m_Preset; }
		inline ShaderID GetShaderID() const { return m_ShaderID; }

		void Update();
		void Bind(VkPipelineLayout pipelineLayout) const;
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

		static std::map<MaterialPresets, VkDescriptorSetLayout> sm_DescriptorSetLayouts;
		static std::map<MaterialPresets, VkDescriptorPool> sm_DescriptorPools;

		std::vector<VkBuffer> m_UniformBuffers;
		std::vector<VkDeviceMemory> m_UniformBuffersMemory;
		std::vector<void*> m_UniformBuffersMapped;
		std::vector<VkDescriptorSet> m_DescriptorSets;

		VkDescriptorSetLayout m_MaterialLayout;
		VkDescriptorPool m_MaterialMemoryPool;
	};

}