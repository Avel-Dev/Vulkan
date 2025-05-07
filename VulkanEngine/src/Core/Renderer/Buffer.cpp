#include "Buffer.h"
#include "BufferUtils.h"
#include "VulkanEngine/VulkanEngine.h"

namespace CHIKU
{
    namespace Buffer
    {
        VertexBuffer::VertexBuffer(VertexLayoutPreset preset)
        {
            m_Layout = Utils::CreateVertexBufferLayout(preset);
            m_VertexBuffer = VK_NULL_HANDLE;
            m_VertexBufferMemory = VK_NULL_HANDLE;

            PrepareBindingDescription();
            PrepareAttributeDescriptions();
        }

        void VertexBuffer::CreateVertexBuffer()
        {
            VkDeviceSize bufferSize = m_Layout.Stride;

            Utils::CreateBuffer(bufferSize,
                VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                m_VertexBuffer, m_VertexBufferMemory);
        }

        void VertexBuffer::SetData(std::vector<void*> vertices)
        {
            VkDeviceSize bufferSize = m_Layout.Stride;

            VkBuffer stagingBuffer;
            VkDeviceMemory stagingBufferMemory;
            Utils::CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                stagingBuffer, stagingBufferMemory);

            void* data;
            vkMapMemory(VulkanEngine::GetDevice(), stagingBufferMemory, 0, bufferSize, 0, &data);
            memcpy(data, vertices.data(), (size_t)bufferSize);
            vkUnmapMemory(VulkanEngine::GetDevice(), stagingBufferMemory);

            Utils::CopyBuffer(stagingBuffer, m_VertexBuffer, bufferSize);

            vkDestroyBuffer(VulkanEngine::GetDevice(), stagingBuffer, nullptr);
            vkFreeMemory(VulkanEngine::GetDevice(), stagingBufferMemory, nullptr);
        }

        void VertexBuffer::Bind(VkCommandBuffer commandBuffer)
        {
            VkBuffer vertexBuffers[] = { m_VertexBuffer };
            VkDeviceSize offsets[] = { 0 };
            vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
        }

        void VertexBuffer::CleanUp()
        {
            vkDestroyBuffer(VulkanEngine::GetDevice(), m_VertexBuffer, nullptr);
            vkFreeMemory(VulkanEngine::GetDevice(),m_VertexBufferMemory, nullptr);
        }

        void VertexBuffer::PrepareBindingDescription()
        {
            m_BindingDescription.binding = m_Binding;
            m_BindingDescription.stride = m_Layout.Stride;
            m_BindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        }

        void VertexBuffer::PrepareAttributeDescriptions()
        {
            m_AttributeDescriptions.clear();
            m_AttributeDescriptions.resize(m_Layout.VertexElements.size());

            for (size_t i = 0; i < m_Layout.VertexElements.size(); i++)
            {
                m_AttributeDescriptions[i].binding = m_Binding;
                m_AttributeDescriptions[i].location = i;
                m_AttributeDescriptions[i].format = Utils::MapVertexAttributeTypeToVkFormat(m_Layout.VertexElements[i].AttributeType);
                m_AttributeDescriptions[i].offset = m_Layout.VertexElements[i].Offset;
            }
        }
    }
}
