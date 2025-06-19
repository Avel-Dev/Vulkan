#pragma once
#include "Asset.h"

namespace CHIKU
{
	class SpriteAsset : public Asset
	{
	public:
		SpriteAsset() : Asset(AssetType::Texture2D) {}
		SpriteAsset(AssetHandle handle) : Asset(handle, AssetType::Texture2D) {}
		SpriteAsset(AssetHandle handle, AssetPath path) : Asset(handle, AssetType::Texture2D, path) {}
	};
}