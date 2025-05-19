#pragma once
#include "VulkanHeader.h"

namespace CHIKU
{
	class IndexBuffer
	{
    public:
        void CreateIndexBuffer(const std::vector<uint32_t>& indices);
        void Bind(VkCommandBuffer commandBuffer);

        void CleanUp();

    private:
        
        VkBuffer m_IndexBuffer;
        VkDeviceMemory m_IndexBufferMemory;
	};
}