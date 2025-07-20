#pragma once
#include "Swapchain.h"
#include "Commands.h"
#include "Window.h"
#include "VulkanHeader.h"

namespace CHIKU
{
	class Renderer
	{
	public:
		Renderer();

		void Init(GLFWwindow* window);
		void CleanUp();
		void Wait();

		static const inline  uint32_t GetCurrentFrame() noexcept { return s_Instance->m_CurrentFrame; }
		static const inline  VkCommandBuffer GetCommandBuffer() noexcept { return s_Instance->m_Commands.GetCommandBuffer(s_Instance->m_CurrentFrame); }
		static const inline  void BeginFrame() noexcept { s_Instance->PrivateBeginFrame(); }
		static const inline  void EndFrame() noexcept { s_Instance->PrivateEndFrame(); }
		static const inline  VkRenderPass& GetRenderPass() noexcept { return s_Instance->m_Swapchain.GetRenderPass(); }
		static const inline  VkPhysicalDevice& GetPhysicalDevice() noexcept { return s_Instance->m_PhysicalDevice; }
		static const inline  VkDevice& GetDevice() noexcept { return s_Instance->m_LogicalDevice; }
		static const inline  VkCommandBuffer BeginRecordingSingleTimeCommands() noexcept { return s_Instance->BeginSingleTimeCommands(); }
		static const inline  void EndRecordingSingleTimeCommands(VkCommandBuffer commandBuffer) noexcept { return s_Instance->EndSingleTimeCommands(commandBuffer); }

	private:
		void PrivateBeginFrame();
		void PrivateEndFrame();

		void BeginRecordingCommands(const VkCommandBuffer& commandBuffer);
		void EndRecordingCommands(const VkCommandBuffer& commandBuffer);
		VkCommandBuffer BeginSingleTimeCommands() { return m_Commands.BeginSingleTimeCommands(); }
		void EndSingleTimeCommands(VkCommandBuffer commandBuffer) { m_Commands.EndSingleTimeCommands(commandBuffer); }
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
		static Renderer* s_Instance;

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

		VkPipelineLayout m_PipelineLayout;
		VkPipeline m_GraphicsPipeline;
	};
}