#pragma once
#include "VulkanHeader.h"
#include "VulkanEngine/Buffer/VertexBuffer.h"

namespace CHIKU
{
	namespace Utils
	{
		void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
		void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
        
        size_t GetAttributeSize(VertexComponentType componentType, VertexAttributeType type);
        void FinalizeLayout(VertexBufferLayout& layout);
		VkFormat GetVkFormat(VertexComponentType component, VertexAttributeType attribute);
	}
}