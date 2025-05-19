#pragma once
#include "VulkanHeader.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"

namespace CHIKU
{
	class Mesh
	{
	public:
		 VkVertexInputBindingDescription GetVertexBufferBindingDescription();
		 std::vector<VkVertexInputAttributeDescription> GetVertexBufferAttributeDescriptions();

	private:
		VertexBuffer m_VertexBuffer;
		IndexBuffer m_IndexBuffer;
	};
}