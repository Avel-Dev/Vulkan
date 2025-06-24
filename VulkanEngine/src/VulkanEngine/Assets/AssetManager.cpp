#include "AssetManager.h"
#include "ModelAsset.h"
#include "MeshAsset.h"
#include "ShaderAsset.h"
#include "MaterialAsset.h"
#include "Utils/Utils.h"

#include <iostream>
#include <fstream>
#include <filesystem>

namespace CHIKU
{
	std::unordered_map<AssetHandle, std::shared_ptr<Asset>> AssetManager::m_Assets;
	std::vector<std::pair<AssetHandle, ShaderHandle>> AssetManager::m_Shader;

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

	AssetHandle AssetManager::AddModel(const AssetPath& path)
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
		std::shared_ptr<MeshAsset> meshAsset = std::make_shared<MeshAsset>(newHandle);
		m_Assets[newHandle] = meshAsset;

		meshAsset->SetMetaData(metaData);
		meshAsset->SetData(data);

		return newHandle;
	}

	AssetHandle AssetManager::AddMaterial(const AssetPath& path)
	{
		ZoneScoped;
		AssetHandle newHandle = Utils::GetRandomNumber<AssetHandle>();
		m_Assets[newHandle] = std::make_shared<MaterialAsset>(newHandle, path);

		return newHandle;
	}

	AssetHandle AssetManager::AddShader(const std::vector<AssetPath>& path)
	{
		ZoneScoped;
		AssetHandle newHandle = Utils::GetRandomNumber<AssetHandle>();
		std::shared_ptr<ShaderAsset> shaderAsset = std::make_shared<ShaderAsset>(newHandle);
		shaderAsset->CreateShader(path);
		m_Assets[newHandle] = shaderAsset;
		m_Shader.emplace_back(newHandle, shaderAsset->GetShaderHandle());
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