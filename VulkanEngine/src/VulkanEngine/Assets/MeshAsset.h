#pragma once
#include "Asset.h"
#include "VulkanEngine/Buffer/VertexBuffer.h"
#include "VulkanEngine/Buffer/IndexBuffer.h"
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
		
		void SetMetaData(VertexBufferMetaData metaData) { m_VertexBuffer.SetMetaData(metaData); }
		void SetData(const std::vector<uint8_t>& data) { m_VertexBuffer.CreateVertexBuffer(data); }
		void SetIndexData(const std::vector<uint32_t>& indices) { m_IndexBuffer.CreateIndexBuffer(indices); }

		virtual void CleanUp() override
		{
			m_VertexBuffer.CleanUp();
			m_IndexBuffer.CleanUp();
			Asset::CleanUp();
		}
		
		inline uint64_t GetVertexCount() const { return m_VertexBuffer.GetCount(); }

		inline VertexBuffer GetVertexBuffer() const { return m_VertexBuffer; }
		inline void Bind() const { m_VertexBuffer.Bind(); };
		void Draw() const;

	private:
		VertexBuffer m_VertexBuffer;
		IndexBuffer m_IndexBuffer;
	};
}