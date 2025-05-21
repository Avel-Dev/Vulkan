#include "IndexBuffer.h"
#include "VulkanEngine/VulkanEngine.h"
#include "BufferUtils.h"

namespace CHIKU
{
    void IndexBuffer::CreateIndexBuffer(const std::vector<uint32_t>& indices)
    {
        count = indices.size();
        VkDeviceSize bufferSize = sizeof(indices[0]) * count;

        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;
        Utils::CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

        void* data;
        vkMapMemory(VulkanEngine::GetDevice(), stagingBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, indices.data(), (size_t)bufferSize);
        vkUnmapMemory(VulkanEngine::GetDevice(), stagingBufferMemory);

        Utils::CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_IndexBuffer, m_IndexBufferMemory);

        Utils::CopyBuffer(stagingBuffer, m_IndexBuffer, bufferSize);

        vkDestroyBuffer(VulkanEngine::GetDevice(), stagingBuffer, nullptr);
        vkFreeMemory(VulkanEngine::GetDevice(), stagingBufferMemory, nullptr);
    }

    void IndexBuffer::Bind()
    {
        vkCmdBindIndexBuffer(VulkanEngine::GetCommandBuffer(), m_IndexBuffer, 0, VK_INDEX_TYPE_UINT32);
    }

    void IndexBuffer::CleanUp()
    {
        vkDestroyBuffer(VulkanEngine::GetDevice(), m_IndexBuffer, nullptr);
        vkFreeMemory(VulkanEngine::GetDevice(), m_IndexBufferMemory, nullptr);
    }
}