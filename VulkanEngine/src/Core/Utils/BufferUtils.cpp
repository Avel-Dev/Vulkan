#include "BufferUtils.h"
#include "VulkanEngine/VulkanEngine.h"
#include "Utils/EngineUtility.h"

namespace CHIKU
{
	namespace Utils
	{
		void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
		{
            ZoneScoped;

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
            ZoneScoped;

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

        size_t GetAttributeSize(VertexAttributeType type)
        {
            ZoneScoped;

            switch (type)
            {
            case VertexAttributeType::Float:     return 4;
            case VertexAttributeType::Vec2:      return 4 * 2;
            case VertexAttributeType::Vec3:      return 4 * 3;
            case VertexAttributeType::Vec4:      return 4 * 4;

            case VertexAttributeType::Int:       return 4;
            case VertexAttributeType::IVec2:     return 4 * 2;
            case VertexAttributeType::IVec3:     return 4 * 3;
            case VertexAttributeType::IVec4:     return 4 * 4;

            case VertexAttributeType::UInt:      return 4;
            case VertexAttributeType::UVec2:     return 4 * 2;
            case VertexAttributeType::UVec3:     return 4 * 3;
            case VertexAttributeType::UVec4:     return 4 * 4;

            case VertexAttributeType::Byte4:     return 1 * 4;
            case VertexAttributeType::Byte4N:    return 1 * 4;
            case VertexAttributeType::UByte4:    return 1 * 4;
            case VertexAttributeType::UByte4N:   return 1 * 4;

            case VertexAttributeType::Short2:    return 2 * 2;
            case VertexAttributeType::Short2N:   return 2 * 2;
            case VertexAttributeType::Short4:    return 2 * 4;
            case VertexAttributeType::Short4N:   return 2 * 4;

            case VertexAttributeType::Unknown:   return 0;
            default:                                     return 0;
            }
        }

        void FinalizeLayout(VertexBufferLayout& layout)
        {
            ZoneScoped;

            uint32_t offset = 0;
            for (auto& attr : layout.VertexElements)
            {
                attr.Offset = offset;
                offset += static_cast<uint32_t>(GetAttributeSize(attr.AttributeType));
            }
            layout.Stride = offset;
        }

        VertexBufferLayout CreateVertexBufferLayout(VertexLayoutPreset preset)
        {
            ZoneScoped;

            VertexBufferLayout layout;

            switch (preset)
            {
            case VertexLayoutPreset::StaticMesh:
                layout.VertexElements = {
                    {VERTEX_FIELD_POSITION, VertexAttributeType::Vec3},
                    {VERTEX_FIELD_NORMAL,   VertexAttributeType::Vec3},
                    {VERTEX_FIELD_TEXCOORD, VertexAttributeType::Vec2}
                };
                break;

            case VertexLayoutPreset::SkinnedMesh:
                layout.VertexElements = {
                    {VERTEX_FIELD_POSITION,  VertexAttributeType::Vec3},
                    {VERTEX_FIELD_NORMAL,    VertexAttributeType::Vec3},
                    {VERTEX_FIELD_TEXCOORD,  VertexAttributeType::Vec2},
                    {VERTEX_FIELD_BONEIDS,  VertexAttributeType::UByte4},
                    {VERTEX_FIELD_WEIGHTS,   VertexAttributeType::Vec4}
                };
                break;

            case VertexLayoutPreset::LitMesh:
                layout.VertexElements = {
                    {VERTEX_FIELD_POSITION, VertexAttributeType::Vec3},
                    {VERTEX_FIELD_NORMAL,   VertexAttributeType::Vec3},
                    {VERTEX_FIELD_TEXCOORD, VertexAttributeType::Vec2},
                    {VERTEX_FIELD_TANGENT,  VertexAttributeType::Vec4}
                };
                break;

            case VertexLayoutPreset::ColoredMesh:
                layout.VertexElements = {
                    {VERTEX_FIELD_POSITION, VertexAttributeType::Vec3},
                    {VERTEX_FIELD_COLOR,    VertexAttributeType::Vec4}
                };
                break;

            case VertexLayoutPreset::DebugLine:
                layout.VertexElements = {
                    {VERTEX_FIELD_POSITION, VertexAttributeType::Vec3},
                    {VERTEX_FIELD_COLOR,    VertexAttributeType::Vec4}
                };
                break;

            case VertexLayoutPreset::PointCloud:
                layout.VertexElements = {
                    {VERTEX_FIELD_POSITION, VertexAttributeType::Vec3}
                };
                break;

            case VertexLayoutPreset::Custom:
                throw std::runtime_error("Custom Vertex Buffer Layout is not supported yet");
                break;

            default:
                throw std::runtime_error("Unsupported Vertex Buffer Layout");
                break;
            }
            Utils::FinalizeLayout(layout);
            return layout;
        }

        VkFormat MapVertexAttributeTypeToVkFormat(VertexAttributeType type)
        {
            ZoneScoped;

            switch (type) {
            case VertexAttributeType::Float:     return VK_FORMAT_R32_SFLOAT;
            case VertexAttributeType::Vec2:      return VK_FORMAT_R32G32_SFLOAT;
            case VertexAttributeType::Vec3:      return VK_FORMAT_R32G32B32_SFLOAT;
            case VertexAttributeType::Vec4:      return VK_FORMAT_R32G32B32A32_SFLOAT;

            case VertexAttributeType::Int:       return VK_FORMAT_R32_SINT;
            case VertexAttributeType::IVec2:     return VK_FORMAT_R32G32_SINT;
            case VertexAttributeType::IVec3:     return VK_FORMAT_R32G32B32_SINT;
            case VertexAttributeType::IVec4:     return VK_FORMAT_R32G32B32A32_SINT;

            case VertexAttributeType::UInt:      return VK_FORMAT_R32_UINT;
            case VertexAttributeType::UVec2:     return VK_FORMAT_R32G32_UINT;
            case VertexAttributeType::UVec3:     return VK_FORMAT_R32G32B32_UINT;
            case VertexAttributeType::UVec4:     return VK_FORMAT_R32G32B32A32_UINT;

            case VertexAttributeType::Byte4:     return VK_FORMAT_R8G8B8A8_SNORM;
            case VertexAttributeType::Byte4N:    return VK_FORMAT_R8G8B8A8_SNORM;
            case VertexAttributeType::UByte4:    return VK_FORMAT_R8G8B8A8_UINT;
            case VertexAttributeType::UByte4N:   return VK_FORMAT_R8G8B8A8_UNORM;

            case VertexAttributeType::Short2:    return VK_FORMAT_R16G16_SINT;
            case VertexAttributeType::Short2N:   return VK_FORMAT_R16G16_SNORM;
            case VertexAttributeType::Short4:    return VK_FORMAT_R16G16B16A16_SINT;
            case VertexAttributeType::Short4N:   return VK_FORMAT_R16G16B16A16_SNORM;

            case VertexAttributeType::Unknown:
            default:                             return VK_FORMAT_UNDEFINED;
            }
        }
	}
}