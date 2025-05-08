#pragma once
#include "Buffer.h"
#include <tiny_obj_loader.h>

namespace CHIKU
{
	class Mesh
	{
	public:
		Mesh();
		void LoadMesh(const tinyobj::attrib_t& attrib,const std::vector<tinyobj::shape_t>& shapes);
	private:
		Buffer::VertexBuffer m_VertexBuffer;

		std::vector<uint8_t> m_VertexData;
		std::vector<uint32_t> m_Indices;
	};
}