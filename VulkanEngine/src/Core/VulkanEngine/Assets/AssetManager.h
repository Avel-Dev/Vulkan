#pragma once
#include "VulkanHeader.h"
#include "Asset.h"
#include <yaml-cpp/yaml.h>

namespace YAML {
	template<>
	struct convert<CHIKU::AssetType> {
		static bool decode(const Node& node, CHIKU::AssetType& rhs) {
			if (!node.IsScalar()) return false;
			rhs = CHIKU::AssetTypeFromString(node.as<std::string>());
			return true;
		}

		static Node encode(const CHIKU::AssetType& rhs) {
			Node node;
			node = CHIKU::AssetTypeToString(rhs);
			return node;
		}
	};
}

namespace CHIKU
{
	class AssetManager
	{
	public:
		static Asset LoadAsset(const AssetHandle& assetHandle);
		static AssetHandle AddAsset(AssetType type,const std::string& path);
		static void Init();

	private:
		YAML::Node m_AssetRegistry;
	};
}