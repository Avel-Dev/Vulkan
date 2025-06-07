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
		ZoneScoped;
		CreateDescriptorPool(); //Create the Descriptor Pool

		GetOrBuildUniform(GenericUniformBuffers::MVP); //Here we Create Uniform Buffer to be used later
		GetOrBuildUniform(GenericUniformBuffers::Color); //Here we Create Uniform Buffer to be used later
	}

	void UniformBuffer::FinalizeLayout(UniformBufferLayout& layout)
	{
		ZoneScoped;

		for (auto attribute : layout.PlainAttributes.Types)
		{
			switch (attribute)
			{
			case UniformPlainDataType::Mat4:
				layout.PlainAttributes.Size += sizeof(glm::mat4);
				break;
			case UniformPlainDataType::Vec3:
				layout.PlainAttributes.Size += sizeof(glm::vec3);
				break;
			}
		}
	}

	UniformBufferLayout UniformBuffer::GetUniformBufferLayout(GenericUniformBuffers BufferType)
	{
		ZoneScoped;

		UniformBufferLayout Layout;
		switch (BufferType)
		{
		case GenericUniformBuffers::MVP:
			Layout = {
				/*UniformPlainData*/
				{
					/*UniformPlainDataType*/
					{
						UniformPlainDataType::Mat4, //Model Matrix
						UniformPlainDataType::Mat4, //View Matrix
						UniformPlainDataType::Mat4  //Projection Matrix
					},
				/*Size = 0*/0, /*FinalizeLayout will get the value*/
				/*Binding = */ 0
			},
				/*UniformOpaqueData*/
				{}
			};
			break;
		case GenericUniformBuffers::Color:
			Layout = {
				/*UniformPlainData*/
				{
					/*UniformPlainDataType*/
					{
						UniformPlainDataType::Vec3, //Model Matrix
					},
				/*Size = 0*/0, /*FinalizeLayout will get the value*/
				/*Binding = */ 0
			},
				/*UniformOpaqueData*/
				{}
			};
			break;
		}

		FinalizeLayout(Layout); //This method calculates the Size of the Uniform Buffer

		return Layout;
	}

	void UniformBuffer::BindDefaultUniforms(VkPipelineLayout pipelineLayout)
	{
		ZoneScoped;

		uint8_t i = 0;
		for (auto& [_,description] : sm_BufferDescriptions)
		{
			vkCmdBindDescriptorSets(VulkanEngine::GetCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, i, 1, &description.DescriptorSets[VulkanEngine::GetCurrentFrame()], 0, nullptr);
			i++;
		}
	}

	void UniformBuffer::Update()
	{
		ZoneScoped;

		static auto startTime = std::chrono::high_resolution_clock::now();

		auto currentTime = std::chrono::high_resolution_clock::now();
		float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

		glm::mat4 model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		glm::mat4 view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float)Window::WIDTH / (float)Window::HEIGHT, 0.1f, 10.0f);

		proj[1][1] *= -1;

		glm::mat4 data[3] = { model,view,proj };
		glm::vec3 color(1.0f, 1.0f, 1.0f);
		memcpy(sm_BufferDescriptions[GenericUniformBuffers::MVP].UniformBuffersMapped[VulkanEngine::GetCurrentFrame()], data, sizeof(glm::mat4) * 3);
		memcpy(sm_BufferDescriptions[GenericUniformBuffers::Color].UniformBuffersMapped[VulkanEngine::GetCurrentFrame()], &color, sizeof(glm::vec3));
	}

	UniformBufferDescription UniformBuffer::GetOrBuildUniform(GenericUniformBuffers presets)
	{
		ZoneScoped;

		if (sm_BufferDescriptions.find(presets) == sm_BufferDescriptions.end())
		{
			sm_BufferDescriptions[presets] = CreateUniformDescription(GetUniformBufferLayout(presets));
		}

		return sm_BufferDescriptions.at(presets);
	}

	UniformBufferDescription UniformBuffer::CreateUniformDescription(const UniformBufferLayout& UniformBufferLayouts, const TextureData* textureData, uint8_t size)
	{
		ZoneScoped;

		UniformBufferDescription description;
		description.DescriptorSetLayouts = CreateDescriptorSetLayout(UniformBufferLayouts);

		if (UniformBufferLayouts.PlainAttributes.Size > 0)
		{
			CreateUniformBuffer(UniformBufferLayouts.PlainAttributes.Size,
				description.UniformBuffers,
				description.UniformBuffersMemory,
				description.UniformBuffersMapped);
		}

		if(UniformBufferLayouts.OpeaquData.size() > 0 && size > 0)
		{
			CreateDescriptorSets(UniformBufferLayouts,
				description.DescriptorSetLayouts,
				description.DescriptorSets,
				description.UniformBuffers,
				textureData,size);
		}
		else
		{
			CreateDescriptorSets(UniformBufferLayouts,
				description.DescriptorSetLayouts,
				description.DescriptorSets,
				description.UniformBuffers);
		}

		return description;
	}

	VkDescriptorSetLayout UniformBuffer::CreateDescriptorSetLayout(const UniformBufferLayout& bufferLayout)
	{
		ZoneScoped;

		VkDescriptorSetLayout layout;

		std::vector<VkDescriptorSetLayoutBinding> bindings{};

		if (bufferLayout.PlainAttributes.Size > 0)
		{
			bindings.emplace_back(
				/*    uint32_t              binding; = */			bufferLayout.PlainAttributes.Binding,
				/*    VkDescriptorType      descriptorType; = */	VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
				/*    uint32_t              descriptorCount; = */	1,
				/*    VkShaderStageFlags    stageFlags; = */		VK_SHADER_STAGE_VERTEX_BIT,
				/*    const VkSampler*      pImmutableSamplers; = */nullptr
			);
		}

		if (bufferLayout.OpeaquData.size() > 0)
		{
			for (auto texture : bufferLayout.OpeaquData)
			{
				bindings.emplace_back(
					/*     uint32_t              binding; =*/			texture.Binding,
					/*     VkDescriptorType      descriptorType; =*/	VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
					/*     uint32_t              descriptorCount; =*/	1,
					/*     VkShaderStageFlags    stageFlags; =*/		VK_SHADER_STAGE_FRAGMENT_BIT,
					/*     const VkSampler*      pImmutableSamplers; =*/nullptr
				); // We will have at least 2 bindings: one for the uniform buffer and one for the texture samplerss
			}
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
		ZoneScoped;

		std::array<VkDescriptorPoolSize, 2> poolSizes{};
		poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		poolSizes[0].descriptorCount = MAX_DESCRIPTOR_SETS;
		poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		poolSizes[1].descriptorCount = MAX_DESCRIPTOR_SETS;

		VkDescriptorPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
		poolInfo.pPoolSizes = poolSizes.data();
		poolInfo.maxSets = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT * MAX_DESCRIPTOR_SETS);

		if (vkCreateDescriptorPool(VulkanEngine::GetDevice(), &poolInfo, nullptr, &sm_DescriptorPool) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create descriptor pool!");
		}
	}

	void UniformBuffer::CreateDescriptorSets(const UniformBufferLayout& layout,
		VkDescriptorSetLayout descriptorSetLayouts,
		std::array<VkDescriptorSet, MAX_FRAMES_IN_FLIGHT>& descriptorSets,
		std::array<VkBuffer, MAX_FRAMES_IN_FLIGHT>& uniformBuffers,
		const TextureData* texturedata,
		uint8_t size)
	{
		ZoneScoped;

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

			if (layout.PlainAttributes.Size > 0)
			{
				VkDescriptorBufferInfo bufferInfo{};
				bufferInfo.buffer = uniformBuffers[i];
				bufferInfo.offset = 0;
				bufferInfo.range = layout.PlainAttributes.Size;

				descriptorWrites.emplace_back(
					/*VkStructureType                  sType; = */ VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
					/*const void*						pNext; = */ nullptr,
					/*VkDescriptorSet                  dstSet; = */ descriptorSets[i],
					/*uint32_t                         dstBinding; = */ layout.PlainAttributes.Binding,
					/*uint32_t                         dstArrayElement; = */ 0,
					/*uint32_t                         descriptorCount; = */ 1,
					/*VkDescriptorType                 descriptorType; = */ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
					/*const VkDescriptorImageInfo* pImageInfo; = */ nullptr,
					/*const VkDescriptorBufferInfo* pBufferInfo; = */ &bufferInfo,
					/*const VkBufferView* pTexelBufferView; = */ nullptr
					);
			}

			if (layout.OpeaquData.size() > 0)
			{
				for (int j = 0; j < layout.OpeaquData.size(); j++)
				{
					VkDescriptorImageInfo imageInfo{};
					imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
					imageInfo.imageView = texturedata[j].TextureImageView;
					imageInfo.sampler = texturedata[j].TextureSampler;

					descriptorWrites.emplace_back(
						/*VkStructureType                  sType; = */ VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
						/*const void*						pNext; = */ nullptr,
						/*VkDescriptorSet                  dstSet; = */ descriptorSets[i],
						/*uint32_t                         dstBinding; = */ layout.OpeaquData[j].Binding,
						/*uint32_t                         dstArrayElement; = */ 0,
						/*uint32_t                         descriptorCount; = */ 1,
						/*VkDescriptorType                 descriptorType; = */ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
						/*const VkDescriptorImageInfo* pImageInfo; = */ &imageInfo,
						/*const VkDescriptorBufferInfo* pBufferInfo; = */ nullptr,
						/*const VkBufferView* pTexelBufferView; = */ nullptr
					);
				}
			}

			vkUpdateDescriptorSets(VulkanEngine::GetDevice(), descriptorWrites.size(), descriptorWrites.data(), 0, nullptr);
		}
	}

	void UniformBuffer::CreateUniformBuffer(size_t Size, 
		std::array<VkBuffer, MAX_FRAMES_IN_FLIGHT>& UniformBuffers , 
		std::array<VkDeviceMemory, MAX_FRAMES_IN_FLIGHT>& UniformBuffersMemory,
		std::array<void*, MAX_FRAMES_IN_FLIGHT>& UniformBuffersMapped)
	{
		ZoneScoped;

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
		ZoneScoped;

		auto& device = VulkanEngine::GetDevice();

		for (auto& [_, description] : sm_BufferDescriptions)
		{

			//vkDestroySampler(device, description.Texture.textureSampler, nullptr);
			//vkDestroyImageView(device, description.Texture.textureImageView, nullptr);

			//vkDestroyImage(device, description.Texture.TextureImage, nullptr);
			//vkFreeMemory(device, description.Texture.TextureImageMemory, nullptr);

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