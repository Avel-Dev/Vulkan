#pragma once
#include "Asset.h"
#include "VulkanEngine/VulkanEngine.h"
#include "VulkanEngine/Buffer/UniformBuffer.h"
#include "VulkanEngine/Buffer/VertexBuffer.h"

#define SHADER_STAGE_VERTEX "Vertex"
#define SHADER_STAGE_TESSELATION "Tesselation"
#define SHADER_STAGE_GEOMETRY "Geometry"
#define SHADER_STAGE_FRAGMENT "Fragment"
#define SHADER_STAGE_COMPUTE "Compute"

#define SHADER_LIT "Lit"
#define SHADER_UNLIT "Unlit"
#define SHADER_DEFAULT_LIT "Defaultlit"
#define SHADER_PBR "PBR"
#define SHADER_MASKED_PBR "MaskedPBR"
#define SHADER_TRANSPARENT_PBR "TransparentPBR"

namespace CHIKU
{
	class ShaderAsset : public Asset
    {
    public:
        ShaderAsset() : Asset(AssetType::Shader) {}
        ShaderAsset(AssetHandle handle) : Asset(handle, AssetType::Shader) {}

        void CreateShader(const std::vector<AssetPath>& shaderCodes);
        bool CreateShaderProgram(const AssetPath& ID);

        const UniformBufferDescription& GetBufferDescription() const 
        { 
            return m_UniformBufferDescription; 
        }
        void CleanUp();
        virtual ~ShaderAsset();

		std::unordered_map<ShaderStages, VkShaderModule>& GetShaderStage() { return m_ShaderStage; }
        ReadableHandle GetShaderHandle() const { return m_ShaderHandle; }

    private:
		void GetShaderNameAndStage(const AssetPath& data, ReadableHandle& name, ShaderStages& stage) const;
        void CreateUniformBufferDescription(const std::vector<AssetPath>& shaderCodes);
        bool CreateSPIRV(const std::string& a_ShaderPath, const std::string& a_OutPutPath) const;
        VkShaderModule CreateShaderModule(const std::vector<char>& code) const;
        std::vector<char> ReadFile(const std::string& filename) const;

		std::vector<AssetPath> m_ShaderCodes;
		std::vector<AssetPath> m_ShaderSPIRVs;

		std::bitset<ATTR_COUNT> m_InputAttributes;
        UniformBufferDescription m_UniformBufferDescription;
        ReadableHandle m_ShaderHandle = "";
        std::unordered_map<ShaderStages,VkShaderModule> m_ShaderStage;
    };
}