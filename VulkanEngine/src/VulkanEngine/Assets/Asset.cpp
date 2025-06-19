#include "Asset.h"
#include "ModelAsset.h"
#include "MeshAsset.h"
#include "SpriteAsset.h"

namespace CHIKU
{
	Asset Asset::CreateAsset(AssetHandle handle, AssetType type, AssetPath path)
	{
		switch (type)
		{
			case AssetType::None:	
				return Asset(handle, type, path);
			case AssetType::Texture2D:
				return SpriteAsset(handle,path);
			case AssetType::TextureCube:
				return SpriteAsset(handle,path);
			case AssetType::Model:
				return ModelAsset(handle, path);
			case AssetType::Shader:
				return ShaderAsset(handle, path);
			case AssetType::Material:
				return MaterialAsset(handle, path);
			case AssetType::Mesh:
				return MeshAsset(handle, path);
			case AssetType::Sound:
				return SoundAsset(handle, path);
		}
	}
}