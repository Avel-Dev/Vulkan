#include "Shader.h"
#include "VulkanEngine/VulkanEngine.h"
#include <iostream>
#include <fstream>
#include <json.hpp>
#include <spirv_reflect.h>

namespace CHIKU
{
    std::unordered_map<std::string, ShaderManager::ShaderProgram> ShaderManager::sm_ShaderPrograms;

    ShaderManager::~ShaderManager() 
    {
        ZoneScoped;

        Cleanup();
    }

    void ShaderManager::Init()
    {
        ZoneScoped;
    }

    std::vector<char> ShaderManager::ReadFile(const std::string& filePath) 
    {
        ZoneScoped;

        std::ifstream file(SOURCE_DIR + filePath, std::ios::ate | std::ios::binary);

        if (!file)
        {
            throw std::runtime_error("Failed to open shader file: " + filePath);
        }

        size_t fileSize = static_cast<size_t>(file.tellg());
        std::vector<char> buffer(fileSize);

        file.seekg(0);
        file.read(buffer.data(), fileSize);
        file.close();
        return buffer;
    }

    VkShaderModule ShaderManager::CreateShaderModule(const std::vector<char>& code) 
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

    bool ShaderManager::GetShaderPath(const std::filesystem::path& ID, std::vector<std::string>& shaderPaths)
    {
        ZoneScoped;

        std::vector<std::string> keys;
        std::string filePath = SOURCE_DIR + "shader/shaderlist.json";
        nlohmann::json shaderJson;
        nlohmann::json* current = &shaderJson;

        //Get the Keys
        for (const auto& part : ID)
        {
            keys.push_back(part.string());
        }

        //Check if Shader List file exists
        if (!std::filesystem::exists(filePath))
        {
            std::cerr << "Error: File does not exist: " << filePath << std::endl;
            return 1;
        }

        //Load Shader List
        std::ifstream inputFile(filePath);
        if (!inputFile.is_open())
        {
            std::cerr << "Error: Could not open file: " << filePath << std::endl;
            return 1;
        }

        //parse the Shader Lists file into Json
        try
        {
            inputFile >> shaderJson;
        }
        catch (const std::exception& e)
        {
            std::cerr << "Error parsing JSON: " << e.what() << std::endl;
            return 1;
        }

        for (const auto& key : keys)
        {
            if (current->contains(key))
            {
                current = &(*current)[key];
            }
            else
            {
                std::cerr << "Key not found: " << key << std::endl;
                return 1;
            }
        }

        if (current->is_array())
        {
            shaderPaths.resize(current->size());
            for (size_t i = 0; i < current->size(); ++i)
            {
                shaderPaths[i] = current->at(i).get<std::string>();
            }
        }
        else
        {
            std::cerr << "Final element is not an array." << std::endl;
            return 0;
        }

        return 1;
    }

    bool ShaderManager::CreateShaderProgram(const std::filesystem::path& ID)
    {
        ZoneScoped;

        if (sm_ShaderPrograms.count(ID.string()))
        {
            return true; // Already loaded
        }

        std::vector<std::string> shaderPaths;

        if (!GetShaderPath(ID,shaderPaths))
        {
            return false;
        }

        ShaderProgram program;
        for (auto path : shaderPaths)
        {
            auto index = path.find_last_of(".");
            CreateSPIRV(path, path + ".spv");
            auto code = ReadFile(path + ".spv");

            if (path.substr(index + 1, path.size()) == "vert")
            {
                program.ShaderModules[ShaderStages::Vertex] = CreateShaderModule(code);
            }
            else if (path.substr(index + 1, path.size()) == "frag")
            {
                program.ShaderModules[ShaderStages::Fragment] = CreateShaderModule(code);
            }
            else if (path.substr(index + 1, path.size()) == "geo")
            {
                program.ShaderModules[ShaderStages::Geometry] = CreateShaderModule(code);
            }
        }

        VkPipelineShaderStageCreateInfo vertStage{};
        vertStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertStage.stage = VK_SHADER_STAGE_VERTEX_BIT;
        vertStage.module = program.ShaderModules[ShaderStages::Vertex];
        vertStage.pName = "main";

        //TODO Add the code for other pipeline shaders

        VkPipelineShaderStageCreateInfo fragStage{};
        fragStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        fragStage.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        fragStage.module = program.ShaderModules[ShaderStages::Fragment];
        fragStage.pName = "main";

        program.Stages = { vertStage, fragStage };
        sm_ShaderPrograms[ID.string()] = program;

        return true;
    }

    const std::vector<VkPipelineShaderStageCreateInfo>& ShaderManager::GetShaderStages(const std::filesystem::path& ID)
    {   
        ZoneScoped;

        if (!CreateShaderProgram(ID))
        {
            throw std::runtime_error("Shader not loaded: " + ID.string());
        }

        return sm_ShaderPrograms[ID.string()].Stages;
    }

    void ShaderManager::Cleanup() 
    {
        ZoneScoped;

        for (auto& [_, program] : sm_ShaderPrograms) 
        {
            for (auto& [_,module] : program.ShaderModules)
            {
                vkDestroyShaderModule(VulkanEngine::GetDevice(), module, nullptr);
            }
        }
        sm_ShaderPrograms.clear();
    }

    bool ShaderManager::CreateSPIRV(const std::string& a_ShaderPath, const std::string& a_OutPutPath)
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