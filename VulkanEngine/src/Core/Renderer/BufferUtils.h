#pragma once
#include "VulkanHeader.h"
#include "VertexBuffer.h"

namespace CHIKU
{
	namespace Utils
	{
		void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
		void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
        
        size_t GetAttributeSize(VertexAttributeType type);
        void FinalizeLayout(VertexBufferLayout& layout);
        VertexBufferLayout CreateVertexBufferLayout(VertexLayoutPreset preset);
        VkFormat MapVertexAttributeTypeToVkFormat(VertexAttributeType type);
	}
}