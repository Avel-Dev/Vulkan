#pragma once
#include "VulkanHeader.h"
#include <array>

namespace CHIKU
{
    namespace Buffer
    {
        class IndexBuffer
        {
        public:
            IndexBuffer(const std::vector<uint32_t>& indices);
            IndexBuffer();

            void CreateIndexBuffer(const std::vector<uint32_t>& indices);

            inline VkVertexInputBindingDescription GetBindingDescription() const { return m_BindingDescription; }
            inline std::array<VkVertexInputAttributeDescription, 3> GetAttributeDescriptions() const { return m_AttributeDescriptions; }
            void Bind(VkCommandBuffer commandBuffer) const;

            void CleanUp();

        private:
            VkVertexInputBindingDescription m_BindingDescription{};
            std::array<VkVertexInputAttributeDescription, 3> m_AttributeDescriptions{};

            VkBuffer m_IndexBuffer;
            VkDeviceMemory m_IndexBufferMemory;
        };
    }
}