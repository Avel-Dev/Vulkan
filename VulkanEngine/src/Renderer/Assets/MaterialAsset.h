#pragma once
#include "Asset.h"
#include "VulkanHeader.h"
#include "ShaderAsset.h"
#include <glm/glm.hpp>

namespace CHIKU
{
	struct Config
	{
		bool depthTest;
		bool depthWrite;
		bool blendEnabled;
		std::string cullMode;
		std::string frontFace;
		std::string polygonMode;
		std::string topology;
		glm::vec4 baseColor;
	};

	struct Material
	{
		ReadableHandle name;
		ReadableHandle shader;
		Config config;
	};

	struct UniformBufferStorage
	{
		std::array<VkBuffer, MAX_FRAMES_IN_FLIGHT> UniformBuffers;
		std::array<VkDeviceMemory, MAX_FRAMES_IN_FLIGHT> UniformBuffersMemory;
		std::array<void*, MAX_FRAMES_IN_FLIGHT> UniformBuffersMapped;
	};

	struct UniformSetStorage
	{
		// key is the binding Index
		std::unordered_map<uint32_t, UniformBufferStorage> BindingStorage;
		VkDescriptorSetLayout DescriptorSetLayout = VK_NULL_HANDLE;
		std::array<VkDescriptorSet, MAX_FRAMES_IN_FLIGHT> DescriptorSets;
	};

	class MaterialAsset : public Asset
	{
	public:
		MaterialAsset() : Asset(AssetType::Material) {}
		MaterialAsset(AssetHandle handle) : Asset(handle, AssetType::Material) {}
		MaterialAsset(AssetHandle handle, AssetPath path) : Asset(handle, AssetType::Material, path) 
		{
			CreateMaterial();
		}

		std::shared_ptr<ShaderAsset> GetShader() const { return m_Shader; }

		std::vector<VkDescriptorSet> GetDescriptorSets(uint32_t frameCount) const { return m_DescriptorSetsChache[frameCount]; };
		void CreateMaterial();
		void CreateUniformBuffer();
		Material LoadMaterialFromFile(const AssetPath& filePath);

		void CleanUp();

		~MaterialAsset();

		std::vector<VkDescriptorSetLayout> GetDescriptorSetLayouts() const;
		Material GetMaterial() const { return m_Material; }
		void UpdateUniformBuffer(uint32_t currentFrame);

		static std::map<uint32_t, UniformSetStorage> CreateDescriptorSetLayout(const UniformBufferDescription& bufferDescription);
		static void CreateDescriptorSets(const UniformBufferDescription& bufferDescription, std::map<uint32_t, UniformSetStorage>& setStorage);

	private:
		std::array< std::vector<VkDescriptorSet>,MAX_FRAMES_IN_FLIGHT> m_DescriptorSetsChache;
		std::shared_ptr<ShaderAsset> m_Shader = nullptr;	
		std::map<uint32_t, UniformSetStorage> m_UniformSetStorage; //Key is the set Index
		Material m_Material;
	};
}