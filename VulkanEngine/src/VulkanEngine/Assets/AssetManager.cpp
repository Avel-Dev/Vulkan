#include "AssetManager.h"
#include "ModelAsset.h"
#include "Utils/Utils.h"
#include <iostream>
#include <fstream>
#include <filesystem>

namespace CHIKU
{
	std::unordered_map<AssetHandle, std::shared_ptr<Asset>> AssetManager::m_Assets;

	void AssetManager::Init()
	{
		ZoneScoped;
	}

	std::shared_ptr<Asset> AssetManager::LoadAsset(const AssetHandle& handle)
	{
		ZoneScoped;
		if (m_Assets.find(handle) != m_Assets.end())
		{
			return m_Assets[handle];
		}
		else
		{
			std::cerr << "Asset with handle " << handle << " not found!" << std::endl;
		}

		return nullptr;
	}

	AssetHandle AssetManager::AddAsset(AssetType type, const std::string& path)
	{
		ZoneScoped;
		
		AssetHandle newHandle = Utils::GetRandomNumber<AssetHandle>();

		switch (type)
		{
		case CHIKU::AssetType::None:
			break;
		case CHIKU::AssetType::Texture2D:
			break;
		case CHIKU::AssetType::TextureCube:
			break;
		case CHIKU::AssetType::Model:
			m_Assets[newHandle] = std::make_shared<ModelAsset>( newHandle, path);
			break;
		case CHIKU::AssetType::Shader:
			break;
		case CHIKU::AssetType::Material:
			break;
		case CHIKU::AssetType::Mesh:
			break;
		case CHIKU::AssetType::Sound:
			break;
		default:
			break;
		}

		return newHandle;
	}
}