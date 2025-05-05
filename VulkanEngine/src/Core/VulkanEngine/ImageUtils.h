#pragma once
#include "VulkanEngine.h"

namespace CHIKU
{
	namespace ImageUtils
	{
		void CreateImage(
			VkPhysicalDevice physicalDevice, 
			VkDevice device, 
			uint32_t width, 
			uint32_t height, 
			VkFormat format, 
			VkImageTiling tiling, 
			VkImageUsageFlags usage, 
			VkMemoryPropertyFlags properties, 
			VkImage& image, 
			VkDeviceMemory& imageMemory);

		VkImageView CreateImageView(VkDevice device, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);
		void TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
		void CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
	}
}