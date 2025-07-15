#pragma once
#include "VulkanHeader.h"
#include "VulkanEngine.h"   

namespace CHIKU
{
	class DescriptorPool
	{
	public:
		static void Init();
		static void CleanUp();	

		static VkDescriptorPool GetDescriptorPool() { return m_DescriptorPool; }

	private:
		static VkDescriptorPool m_DescriptorPool;
	};
}