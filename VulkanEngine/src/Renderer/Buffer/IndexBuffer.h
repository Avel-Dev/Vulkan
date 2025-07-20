#pragma once
#include "VulkanHeader.h"
#include "EngineHeader.h"

namespace CHIKU
{
	class IndexBuffer
	{
    public:
        void CreateIndexBuffer(const std::vector<uint32_t>& indices);
        void Bind() const;

        uint32_t GetCount() const { return count; }
        void CleanUp();

    private:
        uint32_t count;
        VkBuffer m_IndexBuffer;
        VkDeviceMemory m_IndexBufferMemory;
	};
}