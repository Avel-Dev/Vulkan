#pragma once
#include "VulkanEngine/Buffer/VertexBuffer.h"
#include <tinygltf/tiny_gltf.h>
#include <iostream>

namespace CHIKU
{
	class MeshAsset : public Asset
	{
	public:
		MeshAsset() : Asset(AssetType::Mesh) {}
		MeshAsset(AssetHandle handle) : Asset(handle, AssetType::Mesh) {}
		MeshAsset(AssetHandle handle, AssetPath path) : Asset(handle, AssetType::Mesh, path) {}
	};
}