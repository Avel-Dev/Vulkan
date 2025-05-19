#include "Material.h"
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
	std::map<MaterialPresets, VkDescriptorSetLayout> Material::m_DescriptorSetLayouts;
	std::map<MaterialPresets, VkDescriptorPool> Material::m_DescriptorPools;

	ShaderID Material::GetMaterialShader(MaterialPresets presets)
	{
		switch (presets)
		{
		case CHIKU::Lit: return ShaderID::Basic;
		case CHIKU::Unlit: return ShaderID::Basic;
		}

		return ShaderID::Basic;
	}

	VkDescriptorSetLayout Material::GetOrBuildDescriptorLayout(MaterialPresets presets)
	{
		if (m_DescriptorSetLayouts.find(presets) != m_DescriptorSetLayouts.end())
		{
			return m_DescriptorSetLayouts[presets];
		}
		return CreateDescriptorSetLayout(presets);
	}

	VkDescriptorSetLayout Material::CreateDescriptorSetLayout(MaterialPresets presets)
	{
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

		m_DescriptorSetLayouts[presets] = nullptr;

		if (vkCreateDescriptorSetLayout(VulkanEngine::GetDevice(), &layoutInfo, nullptr, &m_DescriptorSetLayouts[presets]) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create descriptor set layout!");
		}
	}

	VkDescriptorPool Material::CreateDescriptorPool(MaterialPresets presets)
	{
		VkDescriptorPoolSize poolSizes{};
		poolSizes.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		poolSizes.descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

		VkDescriptorPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.poolSizeCount = 1;
		poolInfo.pPoolSizes = &poolSizes;
		poolInfo.maxSets = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

		m_DescriptorPools[presets] = nullptr;

		if (vkCreateDescriptorPool(VulkanEngine::GetDevice(), &poolInfo, nullptr, &m_DescriptorPools[presets]) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create descriptor pool!");
		}
	}

	void Material::CreateDescriptorSets()
	{
		std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, m_DescriptorSetLayouts[m_Preset]);
		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = m_DescriptorPools[m_Preset];
		allocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
		allocInfo.pSetLayouts = layouts.data();

		m_DescriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
		if (vkAllocateDescriptorSets(VulkanEngine::GetDevice(), &allocInfo, m_DescriptorSets.data()) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to allocate descriptor sets!");
		}

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			VkDescriptorBufferInfo bufferInfo{};
			bufferInfo.buffer = m_UniformBuffers[i];
			bufferInfo.offset = 0;
			bufferInfo.range = sizeof(UniformBufferObject);

			VkWriteDescriptorSet descriptorWrites{};

			descriptorWrites.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites.dstSet = m_DescriptorSets[i];
			descriptorWrites.dstBinding = 0;
			descriptorWrites.dstArrayElement = 0;
			descriptorWrites.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrites.descriptorCount = 1;
			descriptorWrites.pBufferInfo = &bufferInfo;

			vkUpdateDescriptorSets(VulkanEngine::GetDevice(), 1, &descriptorWrites, 0, nullptr);
		}
	}

	void Material::CreateUniformBuffer()
	{
		VkDeviceSize bufferSize = sizeof(UniformBufferObject);
		m_UniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
		m_UniformBuffersMemory.resize(MAX_FRAMES_IN_FLIGHT);
		m_UniformBuffersMapped.resize(MAX_FRAMES_IN_FLIGHT);

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			Utils::CreateBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
				m_UniformBuffers[i], m_UniformBuffersMemory[i]);

			vkMapMemory(VulkanEngine::GetDevice(), m_UniformBuffersMemory[i], 0, bufferSize, 0, &m_UniformBuffersMapped[i]);
		}
	}

	void Material::CreateMaterial(MaterialPresets presets)
	{
		m_Preset = presets;
		m_ShaderID = GetMaterialShader(m_Preset);
		m_MaterialLayout = Material::GetOrBuildDescriptorLayout(m_Preset);
		m_MaterialMemoryPool = Material::CreateDescriptorPool(m_Preset);
		CreateUniformBuffer();
		CreateDescriptorSets();
	}

	void Material::Update()
	{
		static auto startTime = std::chrono::high_resolution_clock::now();

		auto currentTime = std::chrono::high_resolution_clock::now();
		float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

		UniformBufferObject ubo{};
		ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		ubo.proj = glm::perspective(glm::radians(45.0f), 800 / (float)600, 0.1f, 10.0f);
		ubo.proj[1][1] *= -1;

		memcpy(m_UniformBuffersMapped[VulkanEngine::GetCurrentFrame()], &ubo, sizeof(ubo));
	}

	void Material::Bind(VkPipelineLayout pipelineLayout)
	{
		vkCmdBindDescriptorSets(VulkanEngine::GetCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &m_DescriptorSets[VulkanEngine::GetCurrentFrame()], 0, nullptr);
	}

	void Material::CleanUp()
	{
		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			vkDestroyBuffer(VulkanEngine::GetDevice(), m_UniformBuffers[i], nullptr);
			vkFreeMemory(VulkanEngine::GetDevice(), m_UniformBuffersMemory[i], nullptr);
		}
	}

	void Material::StaticCleanUp()
	{
		for (auto& i : m_DescriptorPools)
		{
			vkDestroyDescriptorPool(VulkanEngine::GetDevice(), i.second, nullptr);
		}

		for (auto& i : m_DescriptorSetLayouts)
		{
			vkDestroyDescriptorSetLayout(VulkanEngine::GetDevice(), i.second, nullptr);
		}
	}
}