#pragma once
#include "VulkanHeader.h"
#include "VulkanEngine/Buffer/VertexBuffer.h"
#include <variant>
#include <filesystem>

namespace CHIKU
{
    class ShaderManager 
    {
    public:
        enum class ShaderStages
        {
            Vertex,
            Tessellation,
            Geometry,
            Fragment,
            Compute
        };

        ~ShaderManager();

        static void Init();

        static bool CreateShaderProgram(const std::filesystem::path& ID);
        static const std::vector<VkPipelineShaderStageCreateInfo>& GetShaderStages(const std::filesystem::path& ID) ;
        static void Cleanup();

    private:
        static bool GetShaderPath(const std::filesystem::path& ID, std::vector<std::string>& shaderPaths);
        static bool CreateSPIRV(const std::string& a_ShaderPath, const std::string& a_OutPutPath);
        static VkShaderModule CreateShaderModule(const std::vector<char>& code);

        static std::vector<char> ReadFile(const std::string& filename);

        struct ShaderProgram 
        {
            std::map<ShaderStages, VkShaderModule> ShaderModules{};
            std::vector<VkPipelineShaderStageCreateInfo> Stages{};
        };

        static std::unordered_map<std::string, ShaderProgram> sm_ShaderPrograms;
    };
}