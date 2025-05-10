#pragma once
#include "VulkanHeader.h"
#include <string>

namespace CHIKU
{
	class Texture
	{
	public:
		Texture(const std::string& path);
		Texture();

		VkImage GetTextureImage() const { return m_TextureImage; }
		VkDeviceMemory GetTextureImageMemory() const { return m_TextureImageMemory; }
		VkImageView GetTextureImageView() const { return m_TextureImageView; }
		VkSampler GetTextureImageSampler() const { return m_TextureSampler; }

		void CreateTexture(const std::string& path);
		void CleanUp();

	private:
		void CreateTextureImage();
		void CreateTextureImageView();
		void CreateTextureSampler();


	private:
		std::string m_Path;

		VkImage m_TextureImage;
		VkDeviceMemory m_TextureImageMemory;
		VkImageView m_TextureImageView;
		VkSampler m_TextureSampler;
	};
}