#include "Material.h"
#include "VulkanEngine/VulkanEngine.h"
#include "Utils/BufferUtils.h"
#include "VulkanEngine/Window.h"
#include "Utils/ImageUtils.h"

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/hash.hpp>

#include <chrono>

namespace CHIKU
{
	std::string Material::GetMaterialShader(MaterialPresets presets)
	{
		ZoneScoped;

		switch (presets)
		{
		case CHIKU::Lit: return "default/lit";
		case CHIKU::Unlit: return "default/unlit";
		}

		return "default/unlit";
	}

	void Material::CreateMaterial(MaterialPresets presets)
	{
		ZoneScoped;

		m_Preset = presets;
		m_ShaderID = GetMaterialShader(m_Preset);
	}

	void Material::Bind(VkPipelineLayout pipelineLayout) const
	{
		ZoneScoped;
		vkCmdBindDescriptorSets(VulkanEngine::GetCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 2, 1, &m_UniformBufferDescription.DescriptorSets[VulkanEngine::GetCurrentFrame()], 0, nullptr);
	}

	void Material::CleanUp()
	{
		ZoneScoped;

		for (int i = 0; i < MAX_TEXTURE_PER_MATERIAL; i++)
		{
			if (m_Textures[i].TextureImageView != VK_NULL_HANDLE)
			{
				vkDestroyImageView(VulkanEngine::GetDevice(), m_Textures[i].TextureImageView, nullptr);
				m_Textures[i].TextureImageView = VK_NULL_HANDLE;
			}
			if (m_Textures[i].TextureSampler != VK_NULL_HANDLE)
			{
				vkDestroySampler(VulkanEngine::GetDevice(), m_Textures[i].TextureSampler, nullptr);
				m_Textures[i].TextureSampler = VK_NULL_HANDLE;
			}
			if (m_Textures[i].TextureImage != VK_NULL_HANDLE)
			{
				vkDestroyImage(VulkanEngine::GetDevice(), m_Textures[i].TextureImage, nullptr);
				m_Textures[i].TextureImage = VK_NULL_HANDLE;
			}
			if (m_Textures[i].TextureImageMemory != VK_NULL_HANDLE)
			{
				vkFreeMemory(VulkanEngine::GetDevice(), m_Textures[i].TextureImageMemory, nullptr);
				m_Textures[i].TextureImageMemory = VK_NULL_HANDLE;
			}
		}
	}

	void Material::CreateTexture(MaterialTextuers textures[MAX_TEXTURE_PER_MATERIAL], uint8_t size)
	{
		for (int i = 0; i < size; i++)
		{
			Utils::CreateImage(
				1024, 1024,
				VK_FORMAT_R8G8B8A8_SRGB,
				VK_IMAGE_TILING_OPTIMAL,
				VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
				m_Textures[i].TextureImage,
				m_Textures[i].TextureImageMemory);

			Utils::CreateTextureImage(textures[i].path, // Path to your texture file
				m_Textures[i].TextureImage,
				m_Textures[i].TextureImageMemory);

			m_Textures[i].TextureImageView = Utils::CreateTextureImageView(m_Textures[i].TextureImage);
			m_Textures[i].TextureSampler = Utils::CreateTextureSampler();
		}

		UniformBufferLayout layout = {
			{},
			{
				{UniformOpaqueDataType::DIFFUSE_TEXTURE,0}
			}
		};

		m_UniformBufferDescription = UniformBuffer::CreateUniformDescription(layout, m_Textures, size);
	}
}