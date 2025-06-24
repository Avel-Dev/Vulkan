#pragma once
#include "Asset.h"

namespace CHIKU
{
	class MaterialAsset : public Asset
	{
	public:
		MaterialAsset() : Asset(AssetType::Material) {}
		MaterialAsset(AssetHandle handle) : Asset(handle, AssetType::Material) {}
		MaterialAsset(AssetHandle handle, AssetPath path) : Asset(handle, AssetType::Material, path) 
		{
			CreateMaterial();
		}

		void CreateMaterial();

	private:
		AssetHandle m_ShaderAssetHandle;
	};
}