#pragma once
#include "Swapchain.h"
#include "Commands.h"
#include "Window.h"
#include "VulkanHeader.h"
#include <optional>

namespace CHIKU
{
	class VulkanEngine
	{
	public:
		static VulkanEngine* s_Instance;
	public:
		VulkanEngine();

		void Init(GLFWwindow* window);
		void CleanUp();
		void BeginFrame();
		void EndFrame();

		static const VkPhysicalDevice& GetPhysicalDevice() { return s_Instance->m_PhysicalDevice; }
		static const VkDevice& GetDevice() { return s_Instance->m_LogicalDevice; }

		void BeginRecordingCommands(const VkCommandBuffer& commandBuffer);
		void EndRecordingCommands(const VkCommandBuffer& commandBuffer);

		VkCommandBuffer BeginSingleTimeCommands() { return m_Commands.BeginSingleTimeCommands(); }
		void EndSingleTimeCommands(VkCommandBuffer commandBuffer) { m_Commands.EndSingleTimeCommands(commandBuffer); }
	private:
		void GetRequiredExtensions();

		static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
			VkDebugUtilsMessageTypeFlagsEXT messageType,
			const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
			void* pUserData);

		void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
		bool CheckValidationLayerSupport();
		void CreateInstance();
		void CreateSurface();
		void CreatePhysicalDevice();

		VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, 
			const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, 
			const VkAllocationCallbacks* pAllocator, 
			VkDebugUtilsMessengerEXT* pDebugMessenger);

		void DestroyDebugUtilsMessengerEXT(VkInstance instance,
			VkDebugUtilsMessengerEXT debugMessenger,
			const VkAllocationCallbacks* pAllocator);

		void SetupDebugMessenger();
		void CreateLogicalDevice();
		void CreateSyncObjects();

	private:
		std::vector<VkSemaphore> m_ImageAvailableSemaphore;
		std::vector<VkSemaphore> m_RenderFinishedSemaphore;
		std::vector<VkFence> m_InFlightFence;

		VkInstance m_Instance;
		VkSurfaceKHR m_Surface;
		VkPhysicalDevice m_PhysicalDevice;
		Commands m_Commands;

		VkDebugUtilsMessengerEXT m_DebugMessenger;
		VkDevice m_LogicalDevice;
		VkQueue m_GraphicsQueue;
		VkQueue m_PresentQueue;

		GLFWwindow* m_Window;

		Swapchain m_Swapchain;
		uint32_t m_ImageIndex = 0;
		uint32_t m_CurrentFrame = 0;

		const std::vector<const char*> m_ValidationLayers = {
			"VK_LAYER_KHRONOS_validation"
		};

		const std::vector<const char*> m_DeviceExtensions = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};

		std::vector<const char*> m_Extension;
	};
}