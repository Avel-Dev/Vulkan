#pragma once
#include "VulkanHeader.h"

namespace CHIKU
{
	namespace Utils
	{
		bool CreateSPIRV(const std::string& a_ShaderPath, const std::string& a_OutPutPath);
		std::vector<char> CreateShaderByteCode(const std::string& a_ShaderPath, const std::string& a_OutPutPath);
		VkShaderModule CreateShaderModule(const std::vector<char>& code);
	}
}