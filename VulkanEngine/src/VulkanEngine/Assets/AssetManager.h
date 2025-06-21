#pragma once
#include "VulkanHeader.h"
#include "Asset.h"
#include <memory.h>

namespace CHIKU
{

	struct VertexBufferMetaData;

	class AssetManager
	{
	public:
		static std::shared_ptr<Asset> LoadAsset(const AssetHandle& assetHandle);
		static AssetHandle AddModel(const std::string& path);
		static AssetHandle AddMesh(const VertexBufferMetaData& metaData, const std::vector<uint8_t>& data);
		static void Init();

		static void CleanUp();

	private:
		static std::unordered_map<AssetHandle, std::shared_ptr<Asset>> m_Assets;
	};
}