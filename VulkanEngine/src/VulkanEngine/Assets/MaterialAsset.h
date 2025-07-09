#pragma once
#include "Asset.h"
#include "VulkanHeader.h"
#include "ShaderAsset.h"

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
	};

	struct Material
	{
		ReadableHandle name;
		ReadableHandle shader;
		Config config;
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

		void Bind(VkPipelineLayout pipelineLayout);

		void CreateMaterial();
		void CreateUniformBuffer();
		Material LoadMaterialFromFile(const AssetPath& filePath);

		void CleanUp();

		~MaterialAsset();

		std::vector<VkDescriptorSetLayout> GetDescriptorSetLayouts() const;

	private:
		void CreateDescriptorSetLayout(const UniformBufferDescription& bufferDescription);
		void CreateDescriptorPool();
		void CreateDescriptorSets();
		void UpdateUniformBuffer(uint32_t currentFrame);

	private:

		struct UniformBufferStorage
		{
			std::vector<uint8_t> DataBuffer;
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

		VkDescriptorPool m_DescriptorPool = VK_NULL_HANDLE;

		std::shared_ptr<ShaderAsset> m_Shader = nullptr;	
		std::map<uint32_t, UniformSetStorage> m_UniformSetStorage; //Key is the set Index
		Material m_Material;
	};
}