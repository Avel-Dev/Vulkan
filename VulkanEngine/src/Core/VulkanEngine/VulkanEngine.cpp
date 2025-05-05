#include "VulkanEngine.h"
#include "EngineUtility.h"

#include <iostream>
#include <cstring>

namespace CHIKU
{
	Commands VulkanEngine::m_Commands;

	VulkanEngine::VulkanEngine()
	{
		m_Instance = VK_NULL_HANDLE;
		m_Surface = VK_NULL_HANDLE;
		m_PhysicalDevice = VK_NULL_HANDLE;

		m_DebugMessenger = VK_NULL_HANDLE;
		m_LogicalDevice = VK_NULL_HANDLE;
		m_GraphicsQueue = VK_NULL_HANDLE;
		m_PresentQueue = VK_NULL_HANDLE;
	}

	void VulkanEngine::Init()
	{
		m_Window.Init();
		GetRequiredExtensions();
		CreateInstance();
		CreateSurface();
		CreatePhysicalDevice();
		CreateLogicalDevice();
		CreateSyncObjects();
		m_Commands.Init(m_GraphicsQueue,m_PhysicalDevice,m_LogicalDevice,m_Surface);
		m_Swapchain.Init(m_Window.GetWindow(),m_PhysicalDevice,m_LogicalDevice,m_Surface);
	}

	void VulkanEngine::CleanUp()
	{
		DestroyDebugUtilsMessengerEXT(m_Instance, m_DebugMessenger, nullptr);

		vkQueueWaitIdle(m_GraphicsQueue);
		vkQueueWaitIdle(m_PresentQueue);

		for (int i = 0;i < MAX_FRAMES_IN_FLIGHT;i++)
		{
			vkDestroySemaphore(m_LogicalDevice, m_ImageAvailableSemaphore[i], nullptr);
			vkDestroySemaphore(m_LogicalDevice, m_RenderFinishedSemaphore[i], nullptr);

			vkWaitForFences(m_LogicalDevice, 1, &m_InFlightFence[i], VK_TRUE, UINT64_MAX);
			vkDestroyFence(m_LogicalDevice, m_InFlightFence[i], nullptr);
		}
		
		vkDeviceWaitIdle(m_LogicalDevice);  // Or vkQueueWaitIdle(queue)

		m_Commands.CleanUp();
		m_Swapchain.CleanUp();
		vkQueueWaitIdle(m_GraphicsQueue);
		vkQueueWaitIdle(m_PresentQueue);
		vkDestroyDevice(m_LogicalDevice, nullptr);
		vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);
		vkDestroyInstance(m_Instance, nullptr);
	}

	void VulkanEngine::GetRequiredExtensions()
	{
		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		m_Extension.insert(m_Extension.end(), glfwExtensions, glfwExtensions + glfwExtensionCount);

#ifdef ENABLE_VALIDATION_LAYERS
		m_Extension.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif
	}

	VKAPI_ATTR VkBool32 VKAPI_CALL VulkanEngine::DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
	{
		std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
		return VK_FALSE;
	}

	void VulkanEngine::PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
	{
		createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		createInfo.pfnUserCallback = DebugCallback;
	}

	bool VulkanEngine::CheckValidationLayerSupport()
	{
		uint32_t layerCount;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

		std::vector<VkLayerProperties> availableLayers(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

		for (const char* layerName : m_ValidationLayers)
		{
			bool layerFound = false;

			for (const auto& layerProperties : availableLayers)
			{
				if (strcmp(layerName, layerProperties.layerName) == 0)
				{
					layerFound = true;
					break;
				}
			}

			if (!layerFound)
			{
				return false;
			}
		}

		return true;
	}

	void VulkanEngine::CreateInstance()
	{
		VkApplicationInfo appInfo{};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "NOGAME";
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = "CHIKU";
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion = VK_API_VERSION_1_0;

		VkInstanceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;
		createInfo.enabledExtensionCount = static_cast<uint32_t>(m_Extension.size());
		createInfo.ppEnabledExtensionNames = m_Extension.data();
		createInfo.enabledLayerCount = 0;

		VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};

#ifdef ENABLE_VALIDATION_LAYERS
		if (!CheckValidationLayerSupport())
		{
			throw std::runtime_error("validation layers requested, but not available!");
		}

		createInfo.enabledLayerCount = static_cast<uint32_t>(m_ValidationLayers.size());
		createInfo.ppEnabledLayerNames = m_ValidationLayers.data();

		PopulateDebugMessengerCreateInfo(debugCreateInfo);
		createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
#endif

		if (vkCreateInstance(&createInfo, nullptr, &m_Instance) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create Vulkan instance!");
		}
	}

	void VulkanEngine::CreateSurface()
	{
		if (glfwCreateWindowSurface(m_Instance, m_Window.GetWindow(), nullptr, &m_Surface) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create window surface!");
		}
	}

	void VulkanEngine::CreatePhysicalDevice()
	{
		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(m_Instance, &deviceCount, nullptr);

		if (deviceCount == 0)
		{
			throw std::runtime_error("failed to find GPUs with Vulkan support!");
		}

		std::vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(m_Instance, &deviceCount, devices.data());
		// Selection logic can be added here

		for (const auto& device : devices)
		{
			if (Utils::IsDeviceSuitable(device, m_Surface, m_DeviceExtensions))
			{
				m_PhysicalDevice = device;
				break;
			}
		}

		if (m_PhysicalDevice == VK_NULL_HANDLE)
		{
			throw std::runtime_error("failed to find a suitable GPU!");
		}

		// Use an ordered map to automatically sort candidates by increasing score
		std::multimap<int, VkPhysicalDevice> candidates;

		for (const auto& device : devices)
		{
			int score = Utils::RateDeviceSuitability(device);
			candidates.insert(std::make_pair(score, device));
		}

		// Check if the best candidate is suitable at all
		if (candidates.rbegin()->first > 0) 
		{
			m_PhysicalDevice = candidates.rbegin()->second;
			VkPhysicalDeviceProperties deviceProperties;

			vkGetPhysicalDeviceProperties(m_PhysicalDevice, &deviceProperties);
			std::cout << "Device Name: " << deviceProperties.deviceName << std::endl;
		}
		else
		{
			throw std::runtime_error("failed to find a suitable GPU!");
		}
	}

	VkResult VulkanEngine::CreateDebugUtilsMessengerEXT(VkInstance instance,
		const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
		const VkAllocationCallbacks* pAllocator,
		VkDebugUtilsMessengerEXT* pDebugMessenger)
	{
		auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
		if (func != nullptr)
		{
			return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
		}
		else
		{
			return VK_ERROR_EXTENSION_NOT_PRESENT;
		}
	}

	void VulkanEngine::DestroyDebugUtilsMessengerEXT(VkInstance instance,
		VkDebugUtilsMessengerEXT debugMessenger, 
		const VkAllocationCallbacks* pAllocator)
	{
		auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
		if (func != nullptr) {
			func(instance, debugMessenger, pAllocator);
		}
	}

	void VulkanEngine::SetupDebugMessenger()
	{
		VkDebugUtilsMessengerCreateInfoEXT createInfo;
		PopulateDebugMessengerCreateInfo(createInfo);

		if (CreateDebugUtilsMessengerEXT(m_Instance, &createInfo, nullptr, &m_DebugMessenger) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to set up debug messenger!");
		}
	}

	void VulkanEngine::CreateLogicalDevice()
	{
#ifdef ENABLE_VALIDATION_LAYERS
		SetupDebugMessenger();
#endif

		VkDeviceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		float queuePriority = 1.0f;

		Utils::QueueFamilyIndices indices = Utils::FindQueueFamilies(m_PhysicalDevice, m_Surface);

		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
		std::set<uint32_t> uniqueQueueFamilies = { indices.GraphicsFamily.value(), indices.PresentFamily.value() };

		for (uint32_t queueFamily : uniqueQueueFamilies) {
			VkDeviceQueueCreateInfo queueCreateInfo{};
			queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfo.queueFamilyIndex = queueFamily;
			queueCreateInfo.queueCount = 1;
			queueCreateInfo.pQueuePriorities = &queuePriority;
			queueCreateInfos.push_back(queueCreateInfo);
		}

		VkPhysicalDeviceFeatures deviceFeatures{};

		createInfo.queueCreateInfoCount = static_cast<uint32_t>(uniqueQueueFamilies.size());
		createInfo.pQueueCreateInfos = queueCreateInfos.data();
		createInfo.enabledExtensionCount = static_cast<uint32_t>(m_DeviceExtensions.size());
		createInfo.ppEnabledExtensionNames = m_DeviceExtensions.data();
		createInfo.pEnabledFeatures = &deviceFeatures;
		createInfo.enabledLayerCount = 0;
		deviceFeatures.samplerAnisotropy = VK_TRUE;

#ifdef ENABLE_VALIDATION_LAYERS
		createInfo.enabledLayerCount = static_cast<uint32_t>(m_ValidationLayers.size());
		createInfo.ppEnabledLayerNames = m_ValidationLayers.data();
#endif // ENABLE_VALIDATION_LAYERS


		if (vkCreateDevice(m_PhysicalDevice, &createInfo, nullptr, &m_LogicalDevice) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create logical device!");
		}

		vkGetDeviceQueue(m_LogicalDevice, indices.GraphicsFamily.value(), 0, &m_GraphicsQueue);
		vkGetDeviceQueue(m_LogicalDevice, indices.PresentFamily.value(), 0, &m_PresentQueue);
	}

	void VulkanEngine::CreateSyncObjects()
	{
		VkSemaphoreCreateInfo semaphoreInfo{};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkFenceCreateInfo fenceInfo{};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		m_ImageAvailableSemaphore.resize(MAX_FRAMES_IN_FLIGHT);
		m_RenderFinishedSemaphore.resize(MAX_FRAMES_IN_FLIGHT);
		m_InFlightFence.resize(MAX_FRAMES_IN_FLIGHT);

		for (int i = 0;i < MAX_FRAMES_IN_FLIGHT;i++)
		{
			if (vkCreateSemaphore(m_LogicalDevice, &semaphoreInfo, nullptr, &m_ImageAvailableSemaphore[i]) != VK_SUCCESS ||
				vkCreateSemaphore(m_LogicalDevice, &semaphoreInfo, nullptr, &m_RenderFinishedSemaphore[i]) != VK_SUCCESS ||
				vkCreateFence(m_LogicalDevice, &fenceInfo, nullptr, &m_InFlightFence[i]) != VK_SUCCESS)
			{
				throw std::runtime_error("failed to create semaphores!");
			}
		}
	}
}