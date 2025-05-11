#include "Shader.h"
#include "VulkanEngine/VulkanEngine.h"
#include <fstream>
#include <iostream>

namespace CHIKU
{
	namespace Utils
	{
		bool CreateSPIRV(const std::string& a_ShaderPath, const std::string& a_OutPutPath)
		{
			std::string command = "glslc.exe " + a_ShaderPath + " -o " + a_OutPutPath;
			int result = std::system(command.c_str());

			if (result != 0)
			{
				std::cerr << "Shader compilation failed with exit code: " << result << std::endl;
				return false;
			}

			std::cout << "Compiled successfully: " << a_OutPutPath << std::endl;
			return true;
		}

		std::vector<char> CreateShaderByteCode(const std::string& a_ShaderPath, const std::string& a_OutPutPath)
		{
			if (!CreateSPIRV(a_ShaderPath, a_OutPutPath))
			{
				return {}; // Return empty vector on failure
			}

			std::ifstream file(a_OutPutPath.c_str(), std::ios::ate | std::ios::binary);
			if (!file.is_open())
			{
				std::cerr << "Failed to open compiled SPIR-V file: " << a_OutPutPath << std::endl;
				return {};
			}

			size_t fileSize = static_cast<size_t>(file.tellg());
			std::vector<char> buffer(fileSize);

			file.seekg(0);
			file.read(buffer.data(), fileSize);
			file.close();

			return buffer;
		}

		VkShaderModule CreateShaderModule(const std::vector<char>& code)
		{

			VkShaderModuleCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
			createInfo.codeSize = code.size();
			createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

			VkShaderModule shaderModule;
			if (vkCreateShaderModule(VulkanEngine::GetDevice(), &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
				throw std::runtime_error("failed to create shader module!");
			}

			return shaderModule;
		}
	}
}