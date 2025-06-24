#pragma once
#include "Asset.h"
#include "VulkanEngine/Buffer/VertexBuffer.h"
#include "VulkanEngine/Buffer/IndexBuffer.h"
#include <tinygltf/tiny_gltf.h>
#include <unordered_map>

namespace CHIKU
{
	

	class ModelAsset : public Asset
	{
	public:
		ModelAsset() : Asset(AssetType::Model) {}
		ModelAsset(AssetHandle handle) : Asset(handle,AssetType::Model) {}
		ModelAsset(AssetHandle handle, AssetPath path) : Asset(handle,AssetType::Model,path) 
		{
			LoadModel(path);
		}

		void CreateMeshes(const tinygltf::Model& model);
		void CreateMaterials(const tinygltf::Model& model);

		bool LoadModel(const AssetPath& path);

	private:

		//The meshes inside the model and the materils for each mesh
		std::unordered_map<AssetHandle, AssetHandle> m_MeshesMaterials;
	};
}