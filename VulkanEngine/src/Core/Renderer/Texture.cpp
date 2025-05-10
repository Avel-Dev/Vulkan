#include "Texture.h"
#include "BufferUtils.h"
#include "ImageUtils.h"
#include "VulkanEngine/VulkanEngine.h"
#include <stb_image.h>
#include <stdexcept>
#include <iostream>

namespace CHIKU
{
    Texture::Texture(const std::string& path) : m_Path(path)
    {
        m_TextureImage = VK_NULL_HANDLE;
        m_TextureImageMemory = VK_NULL_HANDLE;
        CreateTextureImage();
        CreateTextureImageView();
        CreateTextureSampler();
    }

    Texture::Texture()
    {
        m_Path = "";
        m_TextureImage = VK_NULL_HANDLE;
        m_TextureImageMemory = VK_NULL_HANDLE;
        m_TextureImageView = VK_NULL_HANDLE;
        m_TextureSampler = VK_NULL_HANDLE;
    }

    void Texture::CreateTexture(const std::string& path)
    {
        m_Path = path;
        m_TextureImage = VK_NULL_HANDLE;
        m_TextureImageMemory = VK_NULL_HANDLE;
        CreateTextureImage();
        CreateTextureImageView();
        CreateTextureSampler();
    }

    void Texture::CleanUp()
    {
        vkDestroySampler(VulkanEngine::GetDevice(), m_TextureSampler, nullptr);
        vkDestroyImageView(VulkanEngine::GetDevice(), m_TextureImageView, nullptr);
        vkDestroyImage(VulkanEngine::GetDevice(), m_TextureImage, nullptr);
        vkFreeMemory(VulkanEngine::GetDevice(), m_TextureImageMemory, nullptr);
    }

    void Texture::CreateTextureImage()
    {
        int texWidth, texHeight, texChannels;
        stbi_uc* pixels = stbi_load((SOURCE_DIR + m_Path).c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
        VkDeviceSize imageSize = texWidth * texHeight * 4;

        if (!pixels)
        {
            throw std::runtime_error("failed to load texture image!");
        }
        else
        {
            std::cout << "Succeded to load texture image!\n";
        }

        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;

        Utils::CreateBuffer(imageSize,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            stagingBuffer, stagingBufferMemory);

        void* data;
        vkMapMemory(VulkanEngine::GetDevice(), stagingBufferMemory, 0, imageSize, 0, &data);
        memcpy(data, pixels, static_cast<size_t>(imageSize));
        vkUnmapMemory(VulkanEngine::GetDevice(), stagingBufferMemory);

        stbi_image_free(pixels);

        Utils::CreateImage(texWidth, texHeight, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_TextureImage, m_TextureImageMemory);

        Utils::TransitionImageLayout(m_TextureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
        Utils::CopyBufferToImage(stagingBuffer, m_TextureImage, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));
        Utils::TransitionImageLayout(m_TextureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

        vkDestroyBuffer(VulkanEngine::GetDevice(), stagingBuffer, nullptr);
        vkFreeMemory(VulkanEngine::GetDevice(), stagingBufferMemory, nullptr);
    }

    void Texture::CreateTextureImageView()
    {
        m_TextureImageView = Utils::CreateImageView(m_TextureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT);
    }

    void Texture::CreateTextureSampler()
    {
        VkSamplerCreateInfo samplerInfo{};
        samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerInfo.magFilter = VK_FILTER_LINEAR;
        samplerInfo.minFilter = VK_FILTER_LINEAR;
        samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.anisotropyEnable = VK_TRUE;

        VkPhysicalDeviceProperties properties{};
        vkGetPhysicalDeviceProperties(VulkanEngine::GetPhysicalDevice(), &properties);

        samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
        samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        samplerInfo.unnormalizedCoordinates = VK_FALSE;
        samplerInfo.compareEnable = VK_FALSE;
        samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
        samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        samplerInfo.mipLodBias = 0.0f;
        samplerInfo.minLod = 0.0f;
        samplerInfo.maxLod = 0.0f;

        if (vkCreateSampler(VulkanEngine::GetDevice(), &samplerInfo, nullptr, &m_TextureSampler) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create texture sampler!");
        }
    }
}