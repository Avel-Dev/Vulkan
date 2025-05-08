#pragma once
#include "VulkanHeader.h"
#include <glm/glm.hpp>
#include <array>
#include <cstddef> // for size_t

namespace CHIKU
{
	namespace Buffer
	{
        enum class VertexAttributeType 
        {
            Float,          // 32-bit float
            Vec2,           // 2 x 32-bit float (e.g., UVs)
            Vec3,           // 3 x 32-bit float (e.g., position, normal)
            Vec4,           // 4 x 32-bit float (e.g., color, tangent, weights)

            Int,            // 32-bit signed int
            IVec2,          // 2 x int
            IVec3,          // 3 x int
            IVec4,          // 4 x int (e.g., bone indices)

            UInt,           // 32-bit unsigned int
            UVec2,          // 2 x uint
            UVec3,          // 3 x uint
            UVec4,          // 4 x uint

            Byte4,          // 4 x unsigned byte (e.g., packed color, often normalized)
            Byte4N,         // 4 x unsigned byte, normalized
            UByte4,         // 4 x unsigned byte
            UByte4N,        // 4 x unsigned byte, normalized

            Short2,         // 2 x signed short
            Short2N,        // 2 x signed short, normalized
            Short4,         // 4 x signed short
            Short4N,        // 4 x signed short, normalized

            Unknown         // For error handling or unrecognized types
        };


        enum class VertexLayoutPreset {
            StaticMesh,     // position, normal, uv
            SkinnedMesh,    // position, normal, uv, boneIDs, weights
            LitMesh,        // position, normal, uv, tangent
            ColoredMesh,    // position, color
            DebugLine,      // position, color (for line rendering or gizmos)
            PointCloud,     // position only
            Custom          // for user-defined or dynamically built layouts (use with caution)
        };

        struct VertexAttribute
        {
            std::string ElementName;
            VertexAttributeType AttributeType;
            uint32_t Offset = 0; // byte offset within vertex struct
        };

        struct VertexBufferLayout
        {
            std::vector<VertexAttribute> VertexElements;
            uint32_t Stride;
        };

		class VertexBuffer
		{
        public:
            VertexBuffer(VertexLayoutPreset preset);

            void SetBinding(uint32_t binding) { m_Binding = binding; }
            void CreateVertexBuffer();
            void SetData(const std::vector<uint8_t>& data);

            inline VertexBufferLayout GetLayout() { return m_Layout; }
            inline VkVertexInputBindingDescription GetBindingDescription() const { return m_BindingDescription; }
            inline std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions() const { return m_AttributeDescriptions; }
            void Bind(VkCommandBuffer commandBuffer);

            void CleanUp();

        private:
            void PrepareBindingDescription();
            void PrepareAttributeDescriptions();

        private:
            uint32_t m_Binding = 0;
            VkVertexInputBindingDescription m_BindingDescription{};
            std::vector<VkVertexInputAttributeDescription> m_AttributeDescriptions{};

            VkBuffer m_VertexBuffer;
            VkDeviceMemory m_VertexBufferMemory;

            VertexBufferLayout m_Layout;
        };
	}
}