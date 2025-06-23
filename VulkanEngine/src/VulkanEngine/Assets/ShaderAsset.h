#pragma once
#include "Asset.h"
#include "VulkanEngine/VulkanEngine.h"

namespace CHIKU
{
	class ShaderAsset : public Asset
    {
    public:
        ShaderAsset() : Asset(AssetType::Shader) {}
        ShaderAsset(AssetHandle handle) : Asset(handle, AssetType::Shader) {}
        ShaderAsset(AssetHandle handle, AssetPath path) : Asset(handle, AssetType::Shader, path) {}

        enum class ShaderStages
        {
            Vertex,
            Tessellation,
            Geometry,
            Fragment,
            Compute
        };

        virtual ~ShaderAsset();

        static bool CreateShaderProgram(const std::filesystem::path& ID);
        static const std::vector<VkPipelineShaderStageCreateInfo>& GetShaderStages(const std::filesystem::path& ID);
        static void CleanUp();

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