#pragma once
#include "VulkanHeader.h"
#include "VulkanEngine/Assets/MaterialAsset.h"
#include "VulkanEngine/Assets/MeshAsset.h"
#include "VulkanEngine/Buffer/VertexBuffer.h"

namespace CHIKU
{
	struct PipelineKey
	{
		AssetHandle MaterialAssetHandle;
		VertexBufferMetaData VertexBufferMetaData;

		bool operator==(const PipelineKey& other) const
		{
			return MaterialAssetHandle == other.MaterialAssetHandle &&
				VertexBufferMetaData == other.VertexBufferMetaData;
		}
	};
}

namespace std {
	template <>
	struct hash<CHIKU::PipelineKey>
	{
		std::size_t operator()(const CHIKU::PipelineKey& key) const
		{
			std::size_t seed = 0;
			CHIKU::Utils::hash_combine(seed, std::hash<CHIKU::AssetHandle>()(key.MaterialAssetHandle));
			CHIKU::Utils::hash_combine(seed, std::hash<CHIKU::VertexBufferMetaData>()(key.VertexBufferMetaData));
			return seed;
		}
	};
}

namespace CHIKU
{
	class GraphicsPipeline
	{
	public:
		GraphicsPipeline() = default;
		
		static void CleanUp();

		static std::pair<VkPipeline, VkPipelineLayout> CreatePipeline(const std::shared_ptr<MaterialAsset>& materialAsset, const VkVertexInputBindingDescription& bindingDescription,const std::vector<VkVertexInputAttributeDescription>& attributeDescription);
		static std::pair<VkPipeline, VkPipelineLayout> GetPipeline(const std::shared_ptr<MaterialAsset>& materialAsset, const std::shared_ptr<MeshAsset>& meshAsset);

	private:
		static std::unordered_map<PipelineKey, std::pair<VkPipeline,VkPipelineLayout>> m_Pipelines;
	};
}
