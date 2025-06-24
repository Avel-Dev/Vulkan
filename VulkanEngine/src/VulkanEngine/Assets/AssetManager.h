#pragma once
#include "VulkanHeader.h"
#include "Asset.h"
#include <memory.h>

namespace CHIKU
{
	struct VertexBufferMetaData;

	class AssetManager
	{
	public:
		static std::shared_ptr<Asset> LoadAsset(const AssetHandle& assetHandle);
		static AssetHandle AddModel(const AssetPath& path);
		static AssetHandle AddMesh(const VertexBufferMetaData& metaData, const std::vector<uint8_t>& data);
		static AssetHandle AddMaterial(const AssetPath& path);
		static AssetHandle AddShader(const std::vector<AssetPath>& path);
		static void Init();

		static void CleanUp();

	private:
		static std::unordered_map<AssetHandle, std::shared_ptr<Asset>> m_Assets;
		static std::vector<std::pair<AssetHandle,ShaderHandle>> m_Shader;
	};
}