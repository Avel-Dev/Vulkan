#pragma once
#include "Asset.h"
#include "VulkanEngine/VulkanEngine.h"
#define SHADER_STAGE_VERTEX "Vertex"
#define SHADER_STAGE_TESSELATION "Tesselation"
#define SHADER_STAGE_GEOMETRY "Geometry"
#define SHADER_STAGE_FRAGMENT "Fragment"
#define SHADER_STAGE_COMPUTE "Compute"


namespace CHIKU
{
    enum class ShaderStages
    {
        Unknown,
        Vertex,
        Tessellation,
        Geometry,
        Fragment,
        Compute
    };

	class ShaderAsset : public Asset
    {
    public:
        ShaderAsset() : Asset(AssetType::Shader) {}
        ShaderAsset(AssetHandle handle) : Asset(handle, AssetType::Shader) {}
        
        void CreateShader(const std::vector<AssetPath>& shaderCodes);

        virtual ~ShaderAsset();

        bool CreateShaderProgram(const AssetPath& ID);
        void CleanUp();
        ShaderHandle GetShaderHandle() const { return m_ShaderHandle; }

    private:
        bool CreateSPIRV(const std::string& a_ShaderPath, const std::string& a_OutPutPath) const;
        VkShaderModule CreateShaderModule(const std::vector<char>& code) const;
        std::vector<char> ReadFile(const std::string& filename) const;
		void GetShaderNameAndStage(const AssetPath& data, ShaderHandle& name, ShaderStages& stage) const;

        ShaderHandle m_ShaderHandle = "";
        std::unordered_map<ShaderStages,VkShaderModule> m_ShaderStage;
    };
}