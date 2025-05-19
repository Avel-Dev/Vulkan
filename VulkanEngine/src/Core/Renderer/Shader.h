#pragma once
#include "VulkanHeader.h"
#include <unordered_map>

namespace CHIKU
{

    enum class ShaderID {
        Basic,
        Phong,
        PBR,
        Shadow,
        Skybox,
        COUNT
    };

    class ShaderManager 
    {
    public:
        ~ShaderManager();

        static void Init();

        static bool LoadShader(ShaderID ID, const std::string& vertPath, const std::string& fragPath);
        static const std::vector<VkPipelineShaderStageCreateInfo>& GetShaderStages(ShaderID ID) ;
        static void Cleanup();

    private:
        static bool CreateSPIRV(const std::string& a_ShaderPath, const std::string& a_OutPutPath);
        static VkShaderModule CreateShaderModule(const std::vector<char>& code);

        static std::vector<char> ReadFile(const std::string& filename);

        struct ShaderProgram 
        {
            VkShaderModule vertModule;
            VkShaderModule fragModule;
            std::vector<VkPipelineShaderStageCreateInfo> stages;
        };

        static std::unordered_map<ShaderID, ShaderProgram> sm_ShaderPrograms;
    };
}