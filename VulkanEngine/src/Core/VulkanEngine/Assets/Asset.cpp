#include "Asset.h"
#include "Model.h"

namespace CHIKU
{
	Asset Asset::CreateAsset(AssetHandle handle, AssetType type, const std::string& path)
	{
		switch (type)
		{
			case AssetType::None:
				return Asset(handle, type, path);
			case AssetType::Texture2D:
				return SpriteAsset(handle,type,path);
			case AssetType::TextureCube:
				return SpriteAsset(handle, type, path);
			case AssetType::Model:
				return ModelAsset(handle, type, path);
			case AssetType::Shader:
				return ShaderAsset(handle, type, path);
			case AssetType::Material:
				return MaterialAsset(handle, type, path);
			case AssetType::Mesh:
				return MeshAsset(handle, type, path);
			case AssetType::Sound:
				return SoundAsset(handle, type, path);
		}
	}
}