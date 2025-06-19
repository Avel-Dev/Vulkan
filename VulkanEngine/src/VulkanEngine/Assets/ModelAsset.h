#pragma once
#include "Asset.h"
#include "VulkanEngine/Buffer/VertexBuffer.h"
#include "VulkanEngine/Buffer/IndexBuffer.h"
#include <unordered_map>

namespace CHIKU
{
	class MaterialAsset : public Asset
	{
	public:
		MaterialAsset() : Asset(AssetType::Material) {}
		MaterialAsset(AssetHandle handle) : Asset(handle, AssetType::Material) {}
		MaterialAsset(AssetHandle handle, AssetPath path) : Asset(handle, AssetType::Material, path) {}
	};

	class ShaderAsset : public Asset
	{
	public:
		ShaderAsset() : Asset(AssetType::Shader) {}
		ShaderAsset(AssetHandle handle) : Asset(handle, AssetType::Shader) {}
		ShaderAsset(AssetHandle handle, AssetPath path) : Asset(handle, AssetType::Shader, path) {}
	};

	class ModelAsset : public Asset
	{
	public:
		ModelAsset() : Asset(AssetType::Model) {}
		ModelAsset(AssetHandle handle) : Asset(handle,AssetType::Model) {}
		ModelAsset(AssetHandle handle, AssetPath path) : Asset(handle,AssetType::Model,path) 
		{
			LoadModel(path);
		}

		bool LoadModel(const AssetPath& path);

	private:

		//The meshes inside the model and the materils for each mesh
		std::unordered_map<AssetHandle, AssetHandle> m_MeshesMaterials;
	};
}