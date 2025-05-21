#pragma once
#include "VulkanHeader.h"
#include <glm/glm.hpp>

namespace CHIKU
{
#define VERTEX_FIELD_POSITION "inPosition"
#define VERTEX_FIELD_NORMAL "inNormal"
#define VERTEX_FIELD_TEXCOORD "inTexcoord"
#define VERTEX_FIELD_BONEIDS "inBoneIDS"
#define VERTEX_FIELD_WEIGHTS "inWeights"
#define VERTEX_FIELD_TANGENT "inTangent"
#define VERTEX_FIELD_COLOR "inColor"

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
        UnLitMesh,
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
        struct VertexInputDescription
        {
            VkVertexInputBindingDescription BindingDescription;
            std::vector<VkVertexInputAttributeDescription> AttributeDescription;
        };

        static void Init();
        void SetLayout(VertexLayoutPreset layout);
        void SetBinding(uint32_t binding) { m_Binding = binding; }
        void CreateVertexBuffer(const std::vector<uint8_t>& vertices);

        VertexInputDescription GetBufferDescription() const { return sm_VertexInputDescription.at(m_Layout); }
        static inline VkVertexInputBindingDescription GetBindingDescription(VertexLayoutPreset preset) { return sm_VertexInputDescription.at(preset).BindingDescription; }
        static inline std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions(VertexLayoutPreset preset) { return sm_VertexInputDescription.at(preset).AttributeDescription; }

        VertexLayoutPreset GetBufferLayout() const noexcept { return m_Layout; }
        void Bind() const;
        void CleanUp();

    private:
        static VertexBufferLayout GetVertexBufferLayout(VertexLayoutPreset layout);
        static void CreatePresetDescription(VertexLayoutPreset preset);
        static void PrepareBindingDescription(VertexLayoutPreset layout, const VertexBufferLayout& bufferLayout);
        static void PrepareAttributeDescriptions(VertexLayoutPreset layout, const VertexBufferLayout& bufferLayout);

    private:
        uint32_t m_Binding = 0;

        static std::map<VertexLayoutPreset, VertexInputDescription> sm_VertexInputDescription;

        VkBuffer m_VertexBuffer;
        VkDeviceMemory m_VertexBufferMemory;

        VertexLayoutPreset m_Layout;
	};
}