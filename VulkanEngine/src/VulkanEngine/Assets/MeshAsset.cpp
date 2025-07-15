#include "MeshAsset.h"
#include "VulkanEngine/VulkanEngine.h"

namespace CHIKU
{
	void MeshAsset::Draw() const
	{
		m_VertexBuffer.Bind();
		if (m_IndexBuffer.GetCount() > 0)
		{
			m_IndexBuffer.Bind();
			vkCmdDrawIndexed(VulkanEngine::GetCommandBuffer(),m_IndexBuffer.GetCount(), 1, 0, 0, 0);
		}
		else
		{
			vkCmdDraw(VulkanEngine::GetCommandBuffer(), (uint32_t)m_VertexBuffer.GetCount(), 1, 0, 0);
		}
	}
}