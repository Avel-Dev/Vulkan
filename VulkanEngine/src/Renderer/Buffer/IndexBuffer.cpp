#include "IndexBuffer.h"
#include "Renderer/Renderer.h"
#include "Utils/BufferUtils.h"

namespace CHIKU
{
    void IndexBuffer::CreateIndexBuffer(const std::vector<uint32_t>& indices)
    {
        ZoneScoped;

        count = (uint32_t)indices.size();
        VkDeviceSize bufferSize = sizeof(indices[0]) * count;

        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;
        Utils::CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

        void* data;
        vkMapMemory(Renderer::GetDevice(), stagingBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, indices.data(), (size_t)bufferSize);
        vkUnmapMemory(Renderer::GetDevice(), stagingBufferMemory);

        Utils::CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_IndexBuffer, m_IndexBufferMemory);

        Utils::CopyBuffer(stagingBuffer, m_IndexBuffer, bufferSize);

        vkDestroyBuffer(Renderer::GetDevice(), stagingBuffer, nullptr);
        vkFreeMemory(Renderer::GetDevice(), stagingBufferMemory, nullptr);
    }

    void IndexBuffer::Bind() const
    {
        ZoneScoped;

        vkCmdBindIndexBuffer(Renderer::GetCommandBuffer(), m_IndexBuffer, 0, VK_INDEX_TYPE_UINT32);
    }

    void IndexBuffer::CleanUp()
    {
        ZoneScoped;

        vkDestroyBuffer(Renderer::GetDevice(), m_IndexBuffer, nullptr);
        vkFreeMemory(Renderer::GetDevice(), m_IndexBufferMemory, nullptr);
    }
}