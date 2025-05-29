#include "UniformBuffer.h"
#include "Utils/ImageUtils.h"
#include "VulkanEngine/VulkanEngine.h"
#include "Utils/BufferUtils.h"

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/hash.hpp>

#include <chrono>

namespace CHIKU
{
	std::unordered_map<GenericUniformBuffers, UniformBufferDescription> UniformBuffer::sm_BufferDescriptions;
	VkDescriptorPool UniformBuffer::sm_DescriptorPool;

	void UniformBuffer::Init()
	{
		CreateDescriptorPool(); //Create the Descriptor Pool

		GetOrBuildUniform(GenericUniformBuffers::MVP); //Here we Create Uniform Buffer to be used later
	}

	void UniformBuffer::FinalizeLayout(UniformBufferLayout& layout)
	{
		for (auto attribute : layout.PlainBufferAttributes)
		{
			switch (attribute.AttributeType)
			{
			case UniformPlainDataType::Mat4:
				layout.Size += sizeof(glm::mat4);
				break;
			}
		}
	}

	UniformBufferLayout UniformBuffer::GetUniformBufferLayout(GenericUniformBuffers BufferType)
	{
		UniformBufferLayout Layout;
		if (BufferType == GenericUniformBuffers::MVP)
		{
			Layout = {
				/* Plain Data Layout = */ {
					{"u_Model",UniformPlainDataType::Mat4, VK_SHADER_STAGE_VERTEX_BIT},
					{"u_View",UniformPlainDataType::Mat4, VK_SHADER_STAGE_VERTEX_BIT},
					{"u_Proj",UniformPlainDataType::Mat4, VK_SHADER_STAGE_VERTEX_BIT}
				},
				/* Opaque Data like Textures */ {
					{"u_Texture",UniformOpaqueDataType::Sampler2D,VK_SHADER_STAGE_FRAGMENT_BIT}
				},
				/*Binding = */ 0, //This is the binding that should be used in the Shaders
				/*Size = */ 0 //This is only here to show the layout the method to calculate the size is below
			};

			FinalizeLayout(Layout); //This method calculates the Size of the Uniform Buffer
		}

		return Layout;
	}

	void UniformBuffer::Bind(VkPipelineLayout pipelineLayout)
	{
		for (auto& [_, description] : sm_BufferDescriptions)
		{
			vkCmdBindDescriptorSets(VulkanEngine::GetCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &description.DescriptorSets[VulkanEngine::GetCurrentFrame()], 0, nullptr);
		}
	}

	void UniformBuffer::Update()
	{
		static auto startTime = std::chrono::high_resolution_clock::now();

		auto currentTime = std::chrono::high_resolution_clock::now();
		float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

		glm::mat4 model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		glm::mat4 view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float)Window::WIDTH / (float)Window::HEIGHT, 0.1f, 10.0f);

		proj[1][1] *= -1;

		glm::mat4 data[3] = { model,view,proj };

		memcpy(sm_BufferDescriptions[GenericUniformBuffers::MVP].UniformBuffersMapped[VulkanEngine::GetCurrentFrame()], data , sizeof(glm::mat4) * 3);
	}

	UniformBufferDescription UniformBuffer::GetOrBuildUniform(GenericUniformBuffers presets)
	{
		if (sm_BufferDescriptions.find(presets) == sm_BufferDescriptions.end())
		{
			sm_BufferDescriptions[presets] = CreateUniformDescription(presets);
		}

		return sm_BufferDescriptions.at(presets);
	}

	UniformBufferDescription UniformBuffer::CreateUniformDescription(GenericUniformBuffers presets)
	{
		UniformBufferDescription description;
		Utils::CreateTextureImage("models/Texture1.png", description.Texture.TextureImage, description.Texture.TextureImageMemory);
		description.Texture.textureImageView = Utils::CreateTextureImageView(description.Texture.TextureImage);
		description.Texture.textureSampler = Utils::CreateTextureSampler();

		description.UniformBufferLayouts = GetUniformBufferLayout(presets);
		description.DescriptorSetLayouts = CreateDescriptorSetLayout(description.UniformBufferLayouts);		

		CreateUniformBuffer(description.UniformBufferLayouts.Size,
			description.UniformBuffers,
			description.UniformBuffersMemory,
			description.UniformBuffersMapped);

		CreateDescriptorSets(description.UniformBufferLayouts,
			description.DescriptorSetLayouts,
			description.DescriptorSets,
			description.UniformBuffers,
			description.Texture);

		return description;
	}

	VkDescriptorSetLayout UniformBuffer::CreateDescriptorSetLayout(const UniformBufferLayout& bufferLayout)
	{
		VkDescriptorSetLayout layout;

		std::vector<VkDescriptorSetLayoutBinding> bindings;

		if (bufferLayout.PlainBufferAttributes.size() > 0)
		{
			bindings.emplace_back();
			bindings[0].binding = 0;
			bindings[0].descriptorCount = 1;
			bindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			bindings[0].pImmutableSamplers = nullptr;
			bindings[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		}

		if (bufferLayout.OpaqueBufferAttributes.size() > 0)
		{
			bindings.emplace_back();
			bindings[1].binding = 1;
			bindings[1].descriptorCount = 1;
			bindings[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			bindings[1].pImmutableSamplers = nullptr;
			bindings[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
		}

		VkDescriptorSetLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
		layoutInfo.pBindings = bindings.data();

		if (vkCreateDescriptorSetLayout(VulkanEngine::GetDevice(), &layoutInfo, nullptr, &layout) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create descriptor set layout!");
		}

		return layout;
	}

	void UniformBuffer::CreateDescriptorPool()
	{
		std::array<VkDescriptorPoolSize, 2> poolSizes{};
		poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		poolSizes[0].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
		poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		poolSizes[1].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

		VkDescriptorPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
		poolInfo.pPoolSizes = poolSizes.data();
		poolInfo.maxSets = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

		if (vkCreateDescriptorPool(VulkanEngine::GetDevice(), &poolInfo, nullptr, &sm_DescriptorPool) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create descriptor pool!");
		}
	}

	void UniformBuffer::CreateDescriptorSets(const UniformBufferLayout& layout,
		VkDescriptorSetLayout descriptorSetLayouts,
		std::array<VkDescriptorSet, MAX_FRAMES_IN_FLIGHT>& descriptorSets,
		std::array<VkBuffer, MAX_FRAMES_IN_FLIGHT>& uniformBuffers,
		/* Remove this */ 
		TextureData texture)
	{
		std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, descriptorSetLayouts);
		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = sm_DescriptorPool;
		allocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
		allocInfo.pSetLayouts = layouts.data();

		if (vkAllocateDescriptorSets(VulkanEngine::GetDevice(), &allocInfo, descriptorSets.data()) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to allocate descriptor sets!");
		}

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			std::vector<VkWriteDescriptorSet> descriptorWrites{};

			if (layout.PlainBufferAttributes.size() > 0)
			{
				VkDescriptorBufferInfo bufferInfo{};
				bufferInfo.buffer = uniformBuffers[i];
				bufferInfo.offset = 0;
				bufferInfo.range = layout.Size;

				descriptorWrites.emplace_back();

				descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				descriptorWrites[0].dstSet = descriptorSets[i];
				descriptorWrites[0].dstBinding = 0;
				descriptorWrites[0].dstArrayElement = 0;
				descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				descriptorWrites[0].descriptorCount = 1;
				descriptorWrites[0].pBufferInfo = &bufferInfo;
			}

			if (layout.OpaqueBufferAttributes.size() > 0)
			{
				VkDescriptorImageInfo imageInfo{};
				imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
				imageInfo.imageView = texture.textureImageView;
				imageInfo.sampler = texture.textureSampler;

				descriptorWrites.emplace_back();

				descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				descriptorWrites[1].dstSet = descriptorSets[i];
				descriptorWrites[1].dstBinding = 1;
				descriptorWrites[1].dstArrayElement = 0;
				descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
				descriptorWrites[1].descriptorCount = 1;
				descriptorWrites[1].pImageInfo = &imageInfo;
			}

			vkUpdateDescriptorSets(VulkanEngine::GetDevice(), descriptorWrites.size(), descriptorWrites.data(), 0, nullptr);
		}
	}

	void UniformBuffer::CreateUniformBuffer(size_t Size, 
		std::array<VkBuffer, MAX_FRAMES_IN_FLIGHT>& UniformBuffers , 
		std::array<VkDeviceMemory, MAX_FRAMES_IN_FLIGHT>& UniformBuffersMemory,
		std::array<void*, MAX_FRAMES_IN_FLIGHT>& UniformBuffersMapped)
	{
		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			Utils::CreateBuffer(Size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
				UniformBuffers[i], UniformBuffersMemory[i]);

			vkMapMemory(VulkanEngine::GetDevice(), UniformBuffersMemory[i], 0, Size, 0, &UniformBuffersMapped[i]);
		}
	}

	void UniformBuffer::CleanUp()
	{
		auto& device = VulkanEngine::GetDevice();

		for (auto& [_, description] : sm_BufferDescriptions)
		{

			vkDestroySampler(device, description.Texture.textureSampler, nullptr);
			vkDestroyImageView(device, description.Texture.textureImageView, nullptr);

			vkDestroyImage(device, description.Texture.TextureImage, nullptr);
			vkFreeMemory(device, description.Texture.TextureImageMemory, nullptr);

			for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
			{
				vkDestroyBuffer(device, description.UniformBuffers[i], nullptr);
				vkFreeMemory(device, description.UniformBuffersMemory[i], nullptr);
			}

			vkDestroyDescriptorSetLayout(device, description.DescriptorSetLayouts, nullptr);
		}

		vkDestroyDescriptorPool(device, sm_DescriptorPool, nullptr);
	}
}