#include "Shader.h"
#include "VulkanEngine/VulkanEngine.h"
#include <iostream>
#include <fstream>

namespace CHIKU
{
    std::unordered_map<ShaderID, ShaderManager::ShaderProgram> ShaderManager::sm_ShaderPrograms;

    ShaderManager::~ShaderManager() 
    {
        Cleanup();
    }

    void ShaderManager::Init()
    {
        LoadShader(ShaderID::Basic, SOURCE_DIR + "shader/shader.vert", SOURCE_DIR + "shader/shader.frag");
        //LoadShader(ShaderID::Phong, SOURCE_DIR + "shader/", SOURCE_DIR + "shader/");
        //LoadShader(ShaderID::PBR, SOURCE_DIR + "shader/", SOURCE_DIR + "shader/");
        //LoadShader(ShaderID::Shadow, SOURCE_DIR + "shader/", SOURCE_DIR + "shader/");
        //LoadShader(ShaderID::Skybox, SOURCE_DIR + "shader/", SOURCE_DIR + "shader/");
    }

    std::vector<char> ShaderManager::ReadFile(const std::string& filename) 
    {
        std::ifstream file(filename, std::ios::ate | std::ios::binary);

        if (!file)
        {
            throw std::runtime_error("Failed to open shader file: " + filename);
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

    bool ShaderManager::LoadShader(ShaderID ID, const std::string& vertPath, const std::string& fragPath)
    {
        if (sm_ShaderPrograms.count(ID))
        {
            return true; // Already loaded
        }

        CreateSPIRV(vertPath, vertPath + ".spv");
        CreateSPIRV(fragPath, fragPath + ".spv");

        auto vertCode = ReadFile(vertPath + ".spv");
        auto fragCode = ReadFile(fragPath + ".spv");

        ShaderProgram program;
        program.vertModule = CreateShaderModule(vertCode);
        program.fragModule = CreateShaderModule(fragCode);

        VkPipelineShaderStageCreateInfo vertStage{};
        vertStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertStage.stage = VK_SHADER_STAGE_VERTEX_BIT;
        vertStage.module = program.vertModule;
        vertStage.pName = "main";

        VkPipelineShaderStageCreateInfo fragStage{};
        fragStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        fragStage.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        fragStage.module = program.fragModule;
        fragStage.pName = "main";

        program.stages = { vertStage, fragStage };
        sm_ShaderPrograms[ID] = program;
        return true;
    }

    const std::vector<VkPipelineShaderStageCreateInfo>& ShaderManager::GetShaderStages(ShaderID ID) 
    {   
        auto it = sm_ShaderPrograms.find(ID);

        if (it == sm_ShaderPrograms.end())
        {
            throw std::runtime_error("Shader not loaded: " + uint32_t(ID));
        }
        else
        {
            return it->second.stages;
        }
    }

    void ShaderManager::Cleanup() 
    {
        for (auto& [_, program] : sm_ShaderPrograms) 
        {
            vkDestroyShaderModule(VulkanEngine::GetDevice(), program.vertModule, nullptr);
            vkDestroyShaderModule(VulkanEngine::GetDevice(), program.fragModule, nullptr);
        }
        sm_ShaderPrograms.clear();
    }

    bool ShaderManager::CreateSPIRV(const std::string& a_ShaderPath, const std::string& a_OutPutPath)
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
}