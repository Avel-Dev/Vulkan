#pragma once
#include "VulkanHeader.h"
#include <glm/glm.hpp>
#include <bitset>

namespace CHIKU
{
	//GLTF attrute order and name
    constexpr std::array<std::string_view, 8> VertexAttributesArray = {
        "POSITION", "NORMAL", "TEXCOORD_0", "TEXCOORD_1", "COLOR_0", "TANGENT" , "JOINTS_0", "WEIGHTS_0"
    };

    enum VertexAttributeBits {
        ATTR_POSITION = 0,
        ATTR_NORMAL,
        ATTR_TEXCOORD_0,
        ATTR_TEXCOORD_1,
        ATTR_COLOR_0,
        ATTR_TANGENT,
        ATTR_JOINTS_0,
        ATTR_WEIGHTS_0,
        ATTR_COUNT
    };

    enum class VertexComponentType : uint8_t
    {
        Float,          // 32-bit float
        Int,            // 32-bit signed int
        Byte,          // 4 x unsigned byte (e.g., packed color, often normalized)
        Short
    };

    enum class VertexAttributeType : uint8_t
    {
        SCALAR,
        Vec2,           // 2 x 32-bit float (e.g., UVs)
        Vec3,           // 3 x 32-bit float (e.g., position, normal)
        Vec4,           // 4 x 32-bit float (e.g., color, tangent, weights)        
    };

    struct VertexAttribute
    {
        uint32_t Offset = 0; // byte offset within vertex struct
        uint8_t size;
        VertexComponentType ComponentType;
        VertexAttributeType AttributeType;
        //1 byte can be squeezed in here 
    };

    struct VertexBufferLayout
    {
        uint32_t Stride;    
        std::bitset<ATTR_COUNT> mask;   
        std::vector<VertexAttribute> VertexElements;
    };

    struct VertexBufferMetaData
    {
        uint64_t Count;            // number of vertices
        VertexBufferLayout Layout;
    };

	class VertexBuffer
	{
    public:
        void CreateVertexBuffer(const std::vector<uint8_t>& vertices);
        void Bind() const;
        void CleanUp();

        void SetBinding(uint32_t binding) { m_Binding = binding; }
		void SetMetaData(const VertexBufferMetaData& metaData) { m_MetaData = metaData; }

        inline VkVertexInputBindingDescription GetBindingDescription() { return m_BindingDescription; }
        inline std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions() { return m_AttributeDescription; }

    private:
        void PrepareBindingDescription(const VertexBufferMetaData& bufferLayout);
        void PrepareAttributeDescriptions(const VertexBufferMetaData& bufferLayout);

    private:
        uint32_t m_Binding = 0;
        VertexBufferMetaData m_MetaData;
        VkBuffer m_VertexBuffer;
        VkDeviceMemory m_VertexBufferMemory;
        VkVertexInputBindingDescription m_BindingDescription;
        std::vector<VkVertexInputAttributeDescription> m_AttributeDescription;
	};
}