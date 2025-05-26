#include "UniformBuffer.h"
#include "VulkanEngine/VulkanEngine.h"
#include "BufferUtils.h"

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/hash.hpp>

#include <chrono>

namespace CHIKU
{
	std::unordered_map<UniformBuffer::GenericUniformBuffers, UniformBuffer::UniformBufferDescription> UniformBuffer::sm_BufferDescriptions;
	VkDescriptorPool UniformBuffer::sm_DescriptorPool;

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

	UniformBuffer::UniformBufferLayout UniformBuffer::GetUniformBufferLayout(GenericUniformBuffers BufferType)
	{
		UniformBufferLayout Layout;
		if (BufferType == GenericUniformBuffers::VP)
		{
			Layout = {
				/* Layout = */ {
					{"u_MVP",UniformPlainDataType::Mat4, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT}
					//{"u_View",UniformPlainDataType::Mat4, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT},
					//{"u_Proj",UniformPlainDataType::Mat4, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT}
				},
				/*Binding = */ 0,
				0
			};

			FinalizeLayout(Layout);
		}

		return Layout;
	}

	void UniformBuffer::Init()
	{
		CreateDescriptorPool();
		GetOrBuildUniform(GenericUniformBuffers::VP);
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

		glm::mat4 mvp = proj * view * model;

		memcpy(sm_BufferDescriptions[GenericUniformBuffers::VP].UniformBuffersMapped[VulkanEngine::GetCurrentFrame()], &mvp, sizeof(mvp));
	}

	UniformBuffer::UniformBufferDescription UniformBuffer::GetOrBuildUniform(UniformBuffer::GenericUniformBuffers presets)
	{
		if (sm_BufferDescriptions.find(presets) == sm_BufferDescriptions.end())
		{
			sm_BufferDescriptions[presets] = CreateUniformDescription(presets);
		}

		return sm_BufferDescriptions.at(presets);
	}

	UniformBuffer::UniformBufferDescription UniformBuffer::CreateUniformDescription(GenericUniformBuffers presets)
	{
		UniformBufferDescription description;
		description.UniformBufferLayouts = GetUniformBufferLayout(presets);
		description.DescriptorSetLayouts = CreateDescriptorSetLayout(presets);
		
		CreateUniformBuffer(description.UniformBufferLayouts.Size,
			description.UniformBuffers,
			description.UniformBuffersMemory,
			description.UniformBuffersMapped);

		CreateDescriptorSets(description.UniformBufferLayouts,
			description.DescriptorSetLayouts,
			description.DescriptorSets, 
			description.UniformBuffers);

		return description;
	}

	VkDescriptorSetLayout UniformBuffer::CreateDescriptorSetLayout(UniformBuffer::GenericUniformBuffers presets)
	{
		VkDescriptorSetLayout layout;

		VkDescriptorSetLayoutBinding uboLayoutBinding{};
		uboLayoutBinding.binding = 0;
		uboLayoutBinding.descriptorCount = 1;
		uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		uboLayoutBinding.pImmutableSamplers = nullptr;
		uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

		VkDescriptorSetLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = 1;
		layoutInfo.pBindings = &uboLayoutBinding;

		if (vkCreateDescriptorSetLayout(VulkanEngine::GetDevice(), &layoutInfo, nullptr, &layout) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create descriptor set layout!");
		}

		return layout;
	}

	void UniformBuffer::CreateDescriptorPool()
	{
		VkDescriptorPoolSize poolSizes{};
		poolSizes.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		poolSizes.descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

		VkDescriptorPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.poolSizeCount = 1;
		poolInfo.pPoolSizes = &poolSizes;
		poolInfo.maxSets = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

		if (vkCreateDescriptorPool(VulkanEngine::GetDevice(), &poolInfo, nullptr, &sm_DescriptorPool) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create descriptor pool!");
		}
	}

	void UniformBuffer::CreateDescriptorSets(const UniformBufferLayout& layout,
		VkDescriptorSetLayout descriptorSetLayouts,
		std::array<VkDescriptorSet, MAX_FRAMES_IN_FLIGHT>& descriptorSets,
		std::array<VkBuffer, MAX_FRAMES_IN_FLIGHT>& uniformBuffers)
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
			VkDescriptorBufferInfo bufferInfo{};
			bufferInfo.buffer = uniformBuffers[i];
			bufferInfo.offset = 0;
			bufferInfo.range = layout.Size;

			VkWriteDescriptorSet descriptorWrites{};

			descriptorWrites.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites.dstSet = descriptorSets[i];
			descriptorWrites.dstBinding = 0;
			descriptorWrites.dstArrayElement = 0;
			descriptorWrites.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrites.descriptorCount = 1;
			descriptorWrites.pBufferInfo = &bufferInfo;

			vkUpdateDescriptorSets(VulkanEngine::GetDevice(), 1, &descriptorWrites, 0, nullptr);
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
		for (auto& [_, description] : sm_BufferDescriptions)
		{
			for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
			{
				vkDestroyBuffer(VulkanEngine::GetDevice(), description.UniformBuffers[i], nullptr);
				vkFreeMemory(VulkanEngine::GetDevice(), description.UniformBuffersMemory[i], nullptr);
			}

			vkDestroyDescriptorSetLayout(VulkanEngine::GetDevice(), description.DescriptorSetLayouts, nullptr);
		}

		vkDestroyDescriptorPool(VulkanEngine::GetDevice(), sm_DescriptorPool, nullptr);
	}
}