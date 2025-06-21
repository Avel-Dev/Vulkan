#pragma once
#include "Asset.h"
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
		
		virtual void SetMetaData(VertexBufferMetaData metaData) final { m_VertexBuffer.SetMetaData(metaData); }
		virtual void SetData(const std::vector<uint8_t>& data) final { m_VertexBuffer.CreateVertexBuffer(data); }

		virtual void CleanUp() override
		{
			m_VertexBuffer.CleanUp();
			Asset::CleanUp();
		}
		
	private:
		VertexBuffer m_VertexBuffer;
	};
}