#include "ShaderAsset.h"
#include <unordered_map>
#include <fstream>
#include <json.hpp>
#include <iostream>

namespace CHIKU
{
    void ShaderAsset::CreateShader(const std::vector<AssetPath>& shaderCodes)
    {
        for (auto& code : shaderCodes)
        {
            CreateShaderProgram(code);
        }
    }

    ShaderAsset::~ShaderAsset()
    {
        ZoneScoped;
        Asset::~Asset();
        CleanUp();
    }

    std::vector<char> ShaderAsset::ReadFile(const std::string& filePath) const
    {
        ZoneScoped;

		std::string fullPath = SOURCE_DIR + filePath;
        std::ifstream file(fullPath, std::ios::ate | std::ios::binary);

        if (!file)
        {
            throw std::runtime_error("Failed to open shader file: " + fullPath);
        }

        size_t fileSize = static_cast<size_t>(file.tellg());
        std::vector<char> buffer(fileSize);

        file.seekg(0);
        file.read(buffer.data(), fileSize);
        file.close();
        return buffer;
    }

    void ShaderAsset::GetShaderNameAndStage(const AssetPath& path, ShaderHandle& shaderName, ShaderStages& shaderStage) const
    {
        shaderName = "Unknown";
        std::string shaderType = "Unknown";

        std::fstream file(SOURCE_DIR + path.string(), std::ios::in | std::ios::out);
        std::string line;
        while (std::getline(file, line))
        {
            if (line.find("//Name:") != std::string::npos)
            {
                shaderName = line.substr(line.find(":") + 1);
                shaderName.erase(0, shaderName.find_first_not_of(" \t")); // trim
            }
            else if (line.find("//Type:") != std::string::npos)
            {
                shaderType = line.substr(line.find(":") + 1);
                shaderType.erase(0, shaderType.find_first_not_of(" \t"));
            }
        }

        if (shaderType == SHADER_STAGE_VERTEX)              shaderStage = ShaderStages::Vertex;
        else if (shaderType == SHADER_STAGE_TESSELATION)    shaderStage = ShaderStages::Tessellation;
        else if (shaderType == SHADER_STAGE_GEOMETRY)       shaderStage = ShaderStages::Geometry;
        else if (shaderType == SHADER_STAGE_FRAGMENT)       shaderStage = ShaderStages::Fragment;
        else if (shaderType == SHADER_STAGE_COMPUTE)        shaderStage = ShaderStages::Compute;
        else shaderStage = ShaderStages::Unknown;
    }

    VkShaderModule ShaderAsset::CreateShaderModule(const std::vector<char>& code) const
    {
        ZoneScoped;

        VkShaderModuleCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = code.size();
        createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

        VkShaderModule shaderModule;
        if (vkCreateShaderModule(VulkanEngine::GetDevice(), &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create shader module");
        }

        return shaderModule;
    }

    bool ShaderAsset::CreateShaderProgram(const AssetPath& shaderPath)
    {
        ZoneScoped;

        std::string path = shaderPath.string();

        auto index = path.find_last_of(".");
        CreateSPIRV(path, path + ".spv");
        auto code = ReadFile(path + ".spv");

        ShaderStages stage = ShaderStages::Unknown;
        ShaderHandle handle = "";
        GetShaderNameAndStage(path, handle, stage);
        if (m_ShaderHandle != "" && m_ShaderHandle != handle)
        {
            throw std::runtime_error("name in the shaders does not match");
        }
        m_ShaderHandle = handle;

        if (stage != ShaderStages::Unknown)
        {
            if (m_ShaderStage.find(stage) != m_ShaderStage.end())
            {
                throw std::runtime_error("Provided multiple shader for same stage");
            }
            m_ShaderStage[stage] = CreateShaderModule(code);
        }
        else
        {
            throw std::runtime_error("Shader does not specify the shader stage");
        }

        return true;
    }

    void ShaderAsset::CleanUp()
    {
        ZoneScoped;

        for (auto& module : m_ShaderStage)
        {
            vkDestroyShaderModule(VulkanEngine::GetDevice(), module.second, nullptr);
        }
        m_ShaderStage.clear();
    }

    bool ShaderAsset::CreateSPIRV(const std::string& a_ShaderPath, const std::string& a_OutPutPath) const
    {
        ZoneScoped;

        std::string command = "glslc.exe " + (SOURCE_DIR + a_ShaderPath) + " -o " + (SOURCE_DIR + a_OutPutPath);
        int result = std::system(command.c_str());

        if (result != 0)
        {
            std::cerr << "Shader compilation failed with exit code: " << result << std::endl;
            return false;
        }

        std::cout << "Compiled successfully: " << a_OutPutPath << std::endl;
        return true;
    }
}