#pragma once
#include "Buffer.h"

namespace CHIKU
{
    namespace Utils
    {
        void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
        void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);

        size_t GetAttributeSize(Buffer::VertexAttributeType type);
        void FinalizeLayout(Buffer::VertexBufferLayout& layout);
        Buffer::VertexBufferLayout CreateVertexBufferLayout(Buffer::VertexLayoutPreset preset);
        VkFormat MapVertexAttributeTypeToVkFormat(Buffer::VertexAttributeType type);
    }
}