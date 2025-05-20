#include "VertexBuffer.h"
#include "VulkanEngine/VulkanEngine.h"
#include "BufferUtils.h"

namespace CHIKU
{
    std::map<VertexLayoutPreset, VertexBuffer::VertexInputDescription> VertexBuffer::sm_VertexInputDescription;

    void VertexBuffer::Init()
    {
        CreatePresetDescription(VertexLayoutPreset::UnLitMesh);
    }

    void VertexBuffer::SetLayout(VertexLayoutPreset layout)
    {
        m_Layout = layout;
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

    VertexBufferLayout VertexBuffer::GetVertexBufferLayout(VertexLayoutPreset layout)
    {
        switch (layout)
        {
        case CHIKU::VertexLayoutPreset::StaticMesh:
            return {
                        {
                            {"inPosition",VertexAttributeType::Vec3},
                            {"inColor",VertexAttributeType::Vec3}
                        }
            };
        case CHIKU::VertexLayoutPreset::SkinnedMesh:
            return {
                        {
                            {"inPosition",VertexAttributeType::Vec3},
                            {"inColor",VertexAttributeType::Vec3}
                        }
            };
        case CHIKU::VertexLayoutPreset::LitMesh:
            return {
                        {
                            {"inPosition",VertexAttributeType::Vec3},
                            {"inColor",VertexAttributeType::Vec3}
                        }
            };
        case CHIKU::VertexLayoutPreset::ColoredMesh:
            return {
                        {
                            {"inPosition",VertexAttributeType::Vec3},
                            {"inColor",VertexAttributeType::Vec3}
                        }
            };        case CHIKU::VertexLayoutPreset::DebugLine:
        case CHIKU::VertexLayoutPreset::PointCloud:
            return {
                        {
                            {"inPosition",VertexAttributeType::Vec3},
                            {"inColor",VertexAttributeType::Vec3}
                        }
            };        case CHIKU::VertexLayoutPreset::Custom:
        default:
            return {
                        {
                            {"inPosition",VertexAttributeType::Vec3},
                            {"inColor",VertexAttributeType::Vec3}
                        }
            };
        }
    }

    void VertexBuffer::CreatePresetDescription(VertexLayoutPreset preset)
    {
        if (sm_VertexInputDescription.find(preset) != sm_VertexInputDescription.end())
        {
            throw std::runtime_error("preset description already created");
        }

        VertexBufferLayout bufferLayout = GetVertexBufferLayout(preset);
        Utils::FinalizeLayout(bufferLayout);
        PrepareBindingDescription(preset, bufferLayout);
        PrepareAttributeDescriptions(preset, bufferLayout);
    }

    void VertexBuffer::PrepareBindingDescription(VertexLayoutPreset layout, const VertexBufferLayout& bufferLayout)
    {
        sm_VertexInputDescription[layout].BindingDescription.binding = 0;
        sm_VertexInputDescription[layout].BindingDescription.stride = bufferLayout.Stride;
        sm_VertexInputDescription[layout].BindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    }

    void VertexBuffer::PrepareAttributeDescriptions(VertexLayoutPreset layout,const VertexBufferLayout& bufferLayout)
    {
        sm_VertexInputDescription[layout].AttributeDescription.resize(bufferLayout.VertexElements.size());

        for (int i = 0; i < sm_VertexInputDescription[layout].AttributeDescription.size(); i++)
        {
            sm_VertexInputDescription[layout].AttributeDescription[i].binding = 0;
            sm_VertexInputDescription[layout].AttributeDescription[i].location = i;
            sm_VertexInputDescription[layout].AttributeDescription[i].format = VK_FORMAT_R32G32B32_SFLOAT;
            sm_VertexInputDescription[layout].AttributeDescription[i].offset = bufferLayout.VertexElements[i].Offset;
        }
    }
}