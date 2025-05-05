#pragma once
#include "VulkanHeader.h"

namespace CHIKU
{
	class Commands
	{
	public:
		void Init(VkQueue graphicsQueue, VkPhysicalDevice physicalDevice, VkDevice device, VkSurfaceKHR surface);
		void CleanUp();

		VkCommandBuffer BeginSingleTimeCommands();
		void EndSingleTimeCommands(VkCommandBuffer commandBuffer);

	private:
		VkQueue m_GraphicsQueue;
		VkDevice m_LogicalDevice;
		VkCommandPool m_CommandPool;
		std::vector<VkCommandBuffer> m_CommandBuffers;
		void CreateCommandBuffer();
		void CreateCommandPool(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);

	};
}