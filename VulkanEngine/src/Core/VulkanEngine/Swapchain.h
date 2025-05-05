#pragma once
#include "VulkanHeader.h"

namespace CHIKU
{
	struct SwapChainSupportDetails
	{
		VkSurfaceCapabilitiesKHR Capabilities;
		std::vector<VkSurfaceFormatKHR> Formats;
		std::vector<VkPresentModeKHR> PresentModes;
	};

	class Swapchain
	{
	public:
		void Init(GLFWwindow* window, VkPhysicalDevice physicalDevice, VkDevice logicalDevice, VkSurfaceKHR surface);
		void CleanUp();
		void RecreateSwapchain(GLFWwindow* window, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);
		void CreateDepthResources(VkPhysicalDevice physicalDevice);

	public:
		SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface);
		VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
		VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
		VkExtent2D ChooseSwapExtent(GLFWwindow* window,const VkSurfaceCapabilitiesKHR& capabilities);
		VkFormat FindDepthFormat(VkPhysicalDevice physicalDevice);

		VkRenderPass m_RenderPass;
		std::vector<VkFramebuffer> m_SwapChainFramebuffers;
		VkFormat m_SwapChainImageFormat;
		VkSwapchainKHR m_SwapChain;
		VkExtent2D m_SwapChainExtent;

	private:
		void CreateRenderpass(VkPhysicalDevice physicalDevice);
		void CreateSwapchain(GLFWwindow* window, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);
		void CreateImageViews();
		void CreateFrameBuffers();

	private:
		VkDevice m_LogicalDevice;

		VkImage m_DepthImage;
		VkDeviceMemory m_DepthImageMemory;
		VkImageView m_DepthImageView;

		std::vector<VkImage> m_SwapChainImages;
		std::vector<VkImageView> m_SwapChainImageViews;
	};
}