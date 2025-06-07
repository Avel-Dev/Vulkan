#include "AssetManager.h"
#include "Utils/Utils.h"
#include <iostream>
#include <fstream>
#include <filesystem>

namespace CHIKU
{
	void AssetManager::Init()
	{
		ZoneScoped;
	}

	Asset AssetManager::LoadAsset(const AssetHandle& handle)
	{
		YAML::Node root;

		// Load existing YAML
		try {
			root = YAML::LoadFile(ASSET_REGISTRY);
		}
		catch (const std::exception& e) {
			std::cerr << "Failed to load YAML: " << e.what() << "\n";
			return Asset();
		}

		// If "assets" doesn't exist, create it as a sequence
		if (!root[handle]) {
			return Asset();
		}

		AssetType type = root[handle]["Type"].as<AssetType>();
		std::string path = root[handle]["Path"].as<std::string>();

		return Asset::LoadAsset(handle, type, path);
	}

	AssetHandle AssetManager::AddAsset(AssetType type, const std::string& path)
	{
		ZoneScoped;

		std::string assetRegistryPath = ASSET_REGISTRY;

		YAML::Node assetNode;
		assetNode["Type"] = type;
		assetNode["Path"] = path;

		if (!std::filesystem::exists(assetRegistryPath)) {
			std::ofstream create(assetRegistryPath); // Creates empty file
		}

		// Load or initialize the registry
		YAML::Node assetRegistry;
		if (std::filesystem::exists(assetRegistryPath)) {
			assetRegistry = YAML::LoadFile(assetRegistryPath);
		}

		// Insert assetNode under a new handle
		AssetHandle newHandle = Utils::GetRandomNumber<AssetHandle>();
		assetRegistry[newHandle].push_back(assetNode);

		// Write updated registry back to file
		std::ofstream outFile(assetRegistryPath);
		outFile << assetRegistry;

		return newHandle;
	}
}