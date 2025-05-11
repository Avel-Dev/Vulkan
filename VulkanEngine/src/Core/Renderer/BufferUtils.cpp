#include "BufferUtils.h"
#include "VulkanEngine/VulkanEngine.h"
#include "VulkanEngine/EngineUtility.h"

namespace CHIKU
{
    namespace Utils
    {
        void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
        {
            auto commandBuffer = VulkanEngine::BeginRecordingSingleTimeCommands();
            VkBufferCopy copyRegion{};
            copyRegion.srcOffset = 0; // Optional
            copyRegion.dstOffset = 0; // Optional
            copyRegion.size = size;
            vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);
            VulkanEngine::EndRecordingSingleTimeCommands(commandBuffer);
        }
        void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory)
        {
            VkBufferCreateInfo bufferInfo{};
            bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
            bufferInfo.size = size;
            bufferInfo.usage = usage;
            bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

            if (vkCreateBuffer(VulkanEngine::GetDevice(), &bufferInfo, nullptr, &buffer) != VK_SUCCESS)
            {
                throw std::runtime_error("failed to create buffer!");
            }

            VkMemoryRequirements memRequirements;
            vkGetBufferMemoryRequirements(VulkanEngine::GetDevice(), buffer, &memRequirements);

            VkMemoryAllocateInfo allocInfo{};
            allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
            allocInfo.allocationSize = memRequirements.size;
            allocInfo.memoryTypeIndex = Utils::FindMemoryType(VulkanEngine::GetPhysicalDevice(), memRequirements.memoryTypeBits, properties);

            if (vkAllocateMemory(VulkanEngine::GetDevice(), &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS)
            {
                throw std::runtime_error("failed to allocate buffer memory!");
            }

            vkBindBufferMemory(VulkanEngine::GetDevice(), buffer, bufferMemory, 0);
        }

        size_t GetAttributeSize(Buffer::VertexAttributeType type)
        {
            switch (type)
            {
            case Buffer::VertexAttributeType::Float:     return 4;
            case Buffer::VertexAttributeType::Vec2:      return 4 * 2;
            case Buffer::VertexAttributeType::Vec3:      return 4 * 3;
            case Buffer::VertexAttributeType::Vec4:      return 4 * 4;

            case Buffer::VertexAttributeType::Int:       return 4;
            case Buffer::VertexAttributeType::IVec2:     return 4 * 2;
            case Buffer::VertexAttributeType::IVec3:     return 4 * 3;
            case Buffer::VertexAttributeType::IVec4:     return 4 * 4;

            case Buffer::VertexAttributeType::UInt:      return 4;
            case Buffer::VertexAttributeType::UVec2:     return 4 * 2;
            case Buffer::VertexAttributeType::UVec3:     return 4 * 3;
            case Buffer::VertexAttributeType::UVec4:     return 4 * 4;

            case Buffer::VertexAttributeType::Byte4:     return 1 * 4;
            case Buffer::VertexAttributeType::Byte4N:    return 1 * 4;
            case Buffer::VertexAttributeType::UByte4:    return 1 * 4;
            case Buffer::VertexAttributeType::UByte4N:   return 1 * 4;

            case Buffer::VertexAttributeType::Short2:    return 2 * 2;
            case Buffer::VertexAttributeType::Short2N:   return 2 * 2;
            case Buffer::VertexAttributeType::Short4:    return 2 * 4;
            case Buffer::VertexAttributeType::Short4N:   return 2 * 4;

            case Buffer::VertexAttributeType::Unknown:   return 0;
            default:                                     return 0;
            }
        }

        void FinalizeLayout(Buffer::VertexBufferLayout& layout)
        {
            uint32_t offset = 0;
            for (auto& attr : layout.VertexElements)
            {
                attr.Offset = offset;
                offset += static_cast<uint32_t>(GetAttributeSize(attr.AttributeType));
            }
            layout.Stride = offset;
        }

        Buffer::VertexBufferLayout CreateVertexBufferLayout(Buffer::VertexLayoutPreset preset)
        {
            Buffer::VertexBufferLayout layout;

            switch (preset)
            {
            case Buffer::VertexLayoutPreset::StaticMesh:
                layout.VertexElements = {
                    {VERTEX_FIELD_POSITION, Buffer::VertexAttributeType::Vec3},
                    {VERTEX_FIELD_NORMAL,   Buffer::VertexAttributeType::Vec3},
                    {VERTEX_FIELD_TEXCOORD, Buffer::VertexAttributeType::Vec2}
                };
                break;

            case Buffer::VertexLayoutPreset::SkinnedMesh:
                layout.VertexElements = {
                    {VERTEX_FIELD_POSITION,  Buffer::VertexAttributeType::Vec3},
                    {VERTEX_FIELD_NORMAL,    Buffer::VertexAttributeType::Vec3},
                    {VERTEX_FIELD_TEXCOORD,  Buffer::VertexAttributeType::Vec2},
                    {VERTEX_FIELD_BONEIDS,  Buffer::VertexAttributeType::UByte4},
                    {VERTEX_FIELD_WEIGHTS,   Buffer::VertexAttributeType::Vec4}
                };
                break;

            case Buffer::VertexLayoutPreset::LitMesh:
                layout.VertexElements = {
                    {VERTEX_FIELD_POSITION, Buffer::VertexAttributeType::Vec3},
                    {VERTEX_FIELD_NORMAL,   Buffer::VertexAttributeType::Vec3},
                    {VERTEX_FIELD_TEXCOORD, Buffer::VertexAttributeType::Vec2},
                    {VERTEX_FIELD_TANGENT,  Buffer::VertexAttributeType::Vec4}
                };
                break;

            case Buffer::VertexLayoutPreset::ColoredMesh:
                layout.VertexElements = {
                    {VERTEX_FIELD_POSITION, Buffer::VertexAttributeType::Vec3},
                    {VERTEX_FIELD_COLOR,    Buffer::VertexAttributeType::Vec4}
                };
                break;

            case Buffer::VertexLayoutPreset::DebugLine:
                layout.VertexElements = {
                    {VERTEX_FIELD_POSITION, Buffer::VertexAttributeType::Vec3},
                    {VERTEX_FIELD_COLOR,    Buffer::VertexAttributeType::Vec4}
                };
                break;

            case Buffer::VertexLayoutPreset::PointCloud:
                layout.VertexElements = {
                    {VERTEX_FIELD_POSITION, Buffer::VertexAttributeType::Vec3}
                };
                break;

            case Buffer::VertexLayoutPreset::Custom:
                throw std::runtime_error("Custom Vertex Buffer Layout is not supported yet");
                break;

            default:
                throw std::runtime_error("Unsupported Vertex Buffer Layout");
                break;
            }
            Utils::FinalizeLayout(layout);
            return layout;
        }

        VkFormat MapVertexAttributeTypeToVkFormat(Buffer::VertexAttributeType type) 
        {
            switch (type) {
            case Buffer::VertexAttributeType::Float:     return VK_FORMAT_R32_SFLOAT;
            case Buffer::VertexAttributeType::Vec2:      return VK_FORMAT_R32G32_SFLOAT;
            case Buffer::VertexAttributeType::Vec3:      return VK_FORMAT_R32G32B32_SFLOAT;
            case Buffer::VertexAttributeType::Vec4:      return VK_FORMAT_R32G32B32A32_SFLOAT;

            case Buffer::VertexAttributeType::Int:       return VK_FORMAT_R32_SINT;
            case Buffer::VertexAttributeType::IVec2:     return VK_FORMAT_R32G32_SINT;
            case Buffer::VertexAttributeType::IVec3:     return VK_FORMAT_R32G32B32_SINT;
            case Buffer::VertexAttributeType::IVec4:     return VK_FORMAT_R32G32B32A32_SINT;

            case Buffer::VertexAttributeType::UInt:      return VK_FORMAT_R32_UINT;
            case Buffer::VertexAttributeType::UVec2:     return VK_FORMAT_R32G32_UINT;
            case Buffer::VertexAttributeType::UVec3:     return VK_FORMAT_R32G32B32_UINT;
            case Buffer::VertexAttributeType::UVec4:     return VK_FORMAT_R32G32B32A32_UINT;

            case Buffer::VertexAttributeType::Byte4:     return VK_FORMAT_R8G8B8A8_SNORM;
            case Buffer::VertexAttributeType::Byte4N:    return VK_FORMAT_R8G8B8A8_SNORM;
            case Buffer::VertexAttributeType::UByte4:    return VK_FORMAT_R8G8B8A8_UINT;
            case Buffer::VertexAttributeType::UByte4N:   return VK_FORMAT_R8G8B8A8_UNORM;

            case Buffer::VertexAttributeType::Short2:    return VK_FORMAT_R16G16_SINT;
            case Buffer::VertexAttributeType::Short2N:   return VK_FORMAT_R16G16_SNORM;
            case Buffer::VertexAttributeType::Short4:    return VK_FORMAT_R16G16B16A16_SINT;
            case Buffer::VertexAttributeType::Short4N:   return VK_FORMAT_R16G16B16A16_SNORM;

            case Buffer::VertexAttributeType::Unknown:
            default:                             return VK_FORMAT_UNDEFINED;
            }
        }
    }
}