#pragma once
#include "VulkanHeader.h"
#include "Texture.h"
#include <tiny_obj_loader.h>
#include <variant>

namespace CHIKU
{
    enum class MaterialLayoutPreset
    {
        None = 0,
        MVP,
        MVPUnlitTextured,
        MVPStandard,
        ModelViewProjection,
    };

    enum class UniformPlainDataType
    {
        Vec2,           // 2 x 32-bit float (e.g., UVs)
        Vec3,           // 3 x 32-bit float (e.g., position, normal)
        Vec4,           // 4 x 32-bit float (e.g., color, tangent, weights)

        IVec2,          // 2 x int
        IVec3,          // 3 x int
        IVec4,          // 4 x int (e.g., bone indices)

        Mat3,
        Mat4,
        Sampler2D
    };

    enum class UniformOpaqueDataType
    {
        Sampler2D,
        Sampler3D,
        SamplerCube
    };

    struct UniformBufferAttribute
    {
        std::string AttributeName;
        std::variant<UniformPlainDataType, UniformOpaqueDataType> AttributeType;
        VkDescriptorType DescriptorType;
        VkShaderStageFlags ShaderStageFlag;

        UniformBufferAttribute(const std::string& name, UniformPlainDataType plain, VkDescriptorType descriptorType, VkShaderStageFlags stage)
            : AttributeName(name), AttributeType(plain), DescriptorType(descriptorType), ShaderStageFlag(stage) {
        }

        UniformBufferAttribute(const std::string& name, UniformOpaqueDataType opaque, VkDescriptorType descriptorType, VkShaderStageFlags stage)
            : AttributeName(name), AttributeType(opaque), DescriptorType(descriptorType), ShaderStageFlag(stage) {
        }
    };

    struct UniformBufferLayout
    {
        std::vector<UniformBufferAttribute> BufferAttributes;
        size_t size;
    };

	class Material
	{
    public:
        ~Material();
        void CreateMaterial(MaterialLayoutPreset preset);
        void Bind(VkCommandBuffer commandBuffer, VkPipelineLayout descriptorSetLayout, uint32_t currentFrame) const;
        VkDescriptorSetLayout GetDescriptorSetLayout() const { return m_Layout; }
        void CleanUp();

    private:
        void CreateUniformBuffer();

        static size_t GetAttributeSize(const UniformPlainDataType& attributeType);
        static UniformBufferLayout GetDescriptorLayoutForPreset(const MaterialLayoutPreset& preset);
        void CreateDescriptorSetLayout();
        void CreateDescriptorPool();
        void CreateDescriptorSets();

        void UpdateUniformBuffer(uint32_t currentImage) const;

    private:
        UniformBufferLayout m_UniformBufferLayout; //
        VkDescriptorSetLayout m_Layout; //
        VkDescriptorPool m_DescriptorPool; //

        std::vector<Texture> m_Textures;
        std::vector<VkDescriptorSet> m_DescriptorSets;
        std::vector<VkBuffer> m_UniformBuffers;
        std::vector<VkDeviceMemory> m_UniformBuffersMemory;
        std::vector<void*> m_UniformBuffersMapped;
	};
}