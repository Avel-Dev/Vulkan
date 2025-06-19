#pragma once
#include "VulkanHeader.h"
#include "Asset.h"
#include <memory.h>

namespace CHIKU
{
	class AssetManager
	{
	public:
		static std::shared_ptr<Asset> LoadAsset(const AssetHandle& assetHandle);
		static AssetHandle AddAsset(AssetType type,const std::string& path);
		static void Init();

	private:
		static std::unordered_map<AssetHandle, std::shared_ptr<Asset>> m_Assets;
	};
}