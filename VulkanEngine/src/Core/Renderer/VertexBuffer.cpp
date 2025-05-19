#include "VertexBuffer.h"
#include "VulkanEngine/VulkanEngine.h"
#include "BufferUtils.h"

namespace CHIKU
{
    void VertexBuffer::Init()
    {
        PrepareBindingDescription();
        PrepareAttributeDescriptions();
    }

    void VertexBuffer::SetLayout(VertexBufferLayout layout)
    {
        m_BufferLayout = layout; 
        Utils::FinalizeLayout(m_BufferLayout);
    }

    void VertexBuffer::CreateVertexBuffer(const std::vector<uint8_t>& vertices)
    {
        VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;
        Utils::CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            stagingBuffer, stagingBufferMemory);

        void* data;
        vkMapMemory(VulkanEngine::GetDevice(), stagingBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, vertices.data(), (size_t)bufferSize);
        vkUnmapMemory(VulkanEngine::GetDevice(), stagingBufferMemory);

        Utils::CreateBuffer(bufferSize,
            VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            m_VertexBuffer, m_VertexBufferMemory);

        Utils::CopyBuffer(stagingBuffer, m_VertexBuffer, bufferSize);

        vkDestroyBuffer(VulkanEngine::GetDevice(), stagingBuffer, nullptr);
        vkFreeMemory(VulkanEngine::GetDevice(), stagingBufferMemory, nullptr);
    }

    void VertexBuffer::Bind(VkCommandBuffer commandBuffer) const
    {
        VkBuffer vertexBuffers[] = { m_VertexBuffer };
        VkDeviceSize offsets[] = { 0 };
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
    }

    void VertexBuffer::CleanUp()
    {
        vkDestroyBuffer(VulkanEngine::GetDevice(), m_VertexBuffer, nullptr);
        vkFreeMemory(VulkanEngine::GetDevice(), m_VertexBufferMemory, nullptr);
    }

    void VertexBuffer::PrepareBindingDescription()
    {
        m_BindingDescription.binding = m_Binding;
        m_BindingDescription.stride = m_BufferLayout.Stride;
        m_BindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    }

    void VertexBuffer::PrepareAttributeDescriptions()
    {
        if (m_BufferLayout.VertexElements.size() == 0)
        {
            throw std::runtime_error("Vertex Elements count is 0");
        }

        m_AttributeDescriptions.resize(m_BufferLayout.VertexElements.size());

        for (int i = 0; i < m_AttributeDescriptions.size(); i++)
        {
            m_AttributeDescriptions[i].binding = m_Binding;
            m_AttributeDescriptions[i].location = i;
            m_AttributeDescriptions[i].format = VK_FORMAT_R32G32B32_SFLOAT;
            m_AttributeDescriptions[i].offset = m_BufferLayout.VertexElements[i].Offset;
        }
    }
}