#include "VertexBuffer.h"
#include "VulkanEngine/VulkanEngine.h"
#include "Utils/BufferUtils.h"

namespace CHIKU
{
    void VertexBuffer::CreateVertexBuffer(const std::vector<uint8_t>& vertices)
    {
        ZoneScoped;

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

    void VertexBuffer::Bind() const
    {
        ZoneScoped;

        VkBuffer vertexBuffers[] = { m_VertexBuffer };
        VkDeviceSize offsets[] = { 0 };
        vkCmdBindVertexBuffers(VulkanEngine::GetCommandBuffer(), 0, 1, vertexBuffers, offsets);
    }

    void VertexBuffer::CleanUp()
    {
        ZoneScoped;

        vkDestroyBuffer(VulkanEngine::GetDevice(), m_VertexBuffer, nullptr);
        vkFreeMemory(VulkanEngine::GetDevice(), m_VertexBufferMemory, nullptr);
    }

    void VertexBuffer::PrepareBindingDescription(const VertexBufferMetaData& bufferLayout)
    {
        ZoneScoped;

        m_BindingDescription.binding = m_Binding;
        m_BindingDescription.stride = bufferLayout.Layout.Stride;
        m_BindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    }

    void VertexBuffer::PrepareAttributeDescriptions(const VertexBufferMetaData& metaData)
    {
        ZoneScoped;

		const auto& layout = metaData.Layout;

        m_AttributeDescription.resize(layout.VertexElements.size());

        for (int i = 0; i < m_AttributeDescription.size(); i++)
        {
            const auto& componentType = layout.VertexElements[i].ComponentType;
            const auto& attributeType = layout.VertexElements[i].AttributeType;
            const auto& offset = layout.VertexElements[i].Offset;

            m_AttributeDescription[i].binding = 0;
            m_AttributeDescription[i].location = i;
            m_AttributeDescription[i].format = Utils::GetVkFormat(componentType, attributeType);
            m_AttributeDescription[i].offset = offset;
        }
    }
}