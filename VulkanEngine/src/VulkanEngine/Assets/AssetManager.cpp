#include "AssetManager.h"
#include "ModelAsset.h"
#include "MeshAsset.h"
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

	AssetHandle AssetManager::AddModel(const std::string& path)
	{
		ZoneScoped;
		AssetHandle newHandle = Utils::GetRandomNumber<AssetHandle>();
		m_Assets[newHandle] = std::make_shared<ModelAsset>( newHandle, path);

		return newHandle;
	}

	AssetHandle AssetManager::AddMesh(const VertexBufferMetaData& metaData ,const std::vector<uint8_t>& data)
	{
		ZoneScoped;

		AssetHandle newHandle = Utils::GetRandomNumber<AssetHandle>();
		m_Assets[newHandle] = std::make_shared<MeshAsset>(newHandle);
		std::shared_ptr<MeshAsset> meshAsset = std::static_pointer_cast<MeshAsset>(m_Assets[newHandle]);

		meshAsset->SetMetaData(metaData);
		meshAsset->SetData(data);

		return newHandle;
	}

	void AssetManager::CleanUp()
	{
		ZoneScoped;
		for (auto& asset : m_Assets)
		{
			asset.second->CleanUp();
		}
		m_Assets.clear();
	}
}