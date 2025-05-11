#pragma once
#include "Buffer.h"
#include "IndexBuffer.h"
#include <tiny_obj_loader.h>

namespace CHIKU
{
	class Mesh
	{
	public:
		Mesh();
		~Mesh();
		void CleanUp();
		void LoadMesh(const tinyobj::attrib_t& attrib,const std::vector<tinyobj::shape_t>& shapes);
		void Bind(VkCommandBuffer commandBuffer) const;
		void Draw(VkCommandBuffer commandBuffer) const;
		const Buffer::VertexBuffer& GetVertexBuffer() const { return m_VertexBuffer; }

	private:
		Buffer::VertexBuffer m_VertexBuffer;
		Buffer::IndexBuffer m_IndexBuffer;

		std::vector<uint8_t> m_VertexData;
		std::vector<uint32_t> m_Indices;
	};
}