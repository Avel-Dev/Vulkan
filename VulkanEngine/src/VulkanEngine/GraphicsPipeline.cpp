#include "GraphicsPipeline.h"

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/hash.hpp>

namespace CHIKU
{
    std::unordered_map<PipelineKey, std::pair<VkPipeline, VkPipelineLayout>> GraphicsPipeline::m_Pipelines;

    std::array<UniformSetStorage, DEFAULT_DESCRIPTOR_SET_LAYOUT_BINDING_COUNT> GraphicsPipeline::m_GlobalUniformSetStorage; //Key is the set Index>
    std::array<VkDescriptorSetLayout, DEFAULT_DESCRIPTOR_SET_LAYOUT_BINDING_COUNT> GraphicsPipeline::m_GlobalDescriptorSetLayouts; //Key is the set Index>
    std::array<std::array<VkDescriptorSet, DEFAULT_DESCRIPTOR_SET_LAYOUT_BINDING_COUNT>, MAX_FRAMES_IN_FLIGHT> GraphicsPipeline::m_GlobalDescriptorSetsChache;

    void GraphicsPipeline::Init()
    {
        ZoneScoped;
        UniformBufferDescription bufferDescription;
        bufferDescription[0][0] =
        {
            {0,UniformOpaqueDataType::none},
            {
                {UniformPlainDataType::Mat4, 0, sizeof(glm::mat4)},
                {UniformPlainDataType::Mat4, sizeof(glm::mat4), sizeof(glm::mat4)},
                {UniformPlainDataType::Mat4, sizeof(glm::mat4) * 2, sizeof(glm::mat4)}
            },
            0,
			sizeof(glm::mat4) * 3,
        };

        bufferDescription[0][0].Stages.set(ShaderStages::Stage_Vertex);

        auto setStorage = MaterialAsset::CreateDescriptorSetLayout(bufferDescription);
        MaterialAsset::CreateDescriptorSets(bufferDescription, setStorage);

        for(const auto& [setIndex,UniformStorage] : setStorage)
        {
            m_GlobalUniformSetStorage[setIndex] = UniformStorage;
            m_GlobalDescriptorSetLayouts[setIndex] = UniformStorage.DescriptorSetLayout;
            for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
            {
                m_GlobalDescriptorSetsChache[i][setIndex] = UniformStorage.DescriptorSets[i];
            }
		}
	}

    std::pair<VkPipeline, VkPipelineLayout> GraphicsPipeline::CreatePipeline(const std::shared_ptr < MaterialAsset>& materialAsset, const VkVertexInputBindingDescription& bindingDescription, const std::vector<VkVertexInputAttributeDescription>& attributeDescription)
	{
        ZoneScoped;

		const auto& config = materialAsset->GetMaterial().config;
		const auto& shaderAsset = materialAsset->GetShader();
        const auto& shaderStages = shaderAsset->GetShaderStage();
        
		std::vector<VkPipelineShaderStageCreateInfo> shaderStageInfos;

        for (const auto& [stage, shaderModule] : shaderStages)
        {
			VkShaderStageFlagBits flags = VK_SHADER_STAGE_ALL; // Set flags if needed

            if(stage == ShaderStages::Stage_Vertex)
            {
                flags = VK_SHADER_STAGE_VERTEX_BIT;
            }
            else if(stage == ShaderStages::Stage_Fragment)
            {
                flags = VK_SHADER_STAGE_FRAGMENT_BIT;
			}

            VkPipelineShaderStageCreateInfo shaderStage{};
            shaderStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            shaderStage.pNext = nullptr;
            shaderStage.flags = 0;
            shaderStage.stage = flags; // e.g., VK_SHADER_STAGE_VERTEX_BIT
            shaderStage.module = shaderModule;
            shaderStage.pName = "main";
            shaderStage.pSpecializationInfo = nullptr;

            shaderStageInfos.push_back(shaderStage);
        }

        VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

        vertexInputInfo.vertexBindingDescriptionCount = 1;
		vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescription.size());
        vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
        vertexInputInfo.pVertexAttributeDescriptions = attributeDescription.data();

        VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
        inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        inputAssembly.primitiveRestartEnable = VK_FALSE;

        VkPipelineViewportStateCreateInfo viewportState{};
        viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.viewportCount = 1;
        viewportState.scissorCount = 1;

        VkPipelineRasterizationStateCreateInfo rasterizer{};
        rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizer.depthClampEnable = VK_FALSE;
        rasterizer.rasterizerDiscardEnable = VK_FALSE;
        rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
        rasterizer.lineWidth = 1.0f;
        rasterizer.cullMode = VK_CULL_MODE_NONE;
        rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
        rasterizer.depthBiasEnable = VK_FALSE;

        VkPipelineMultisampleStateCreateInfo multisampling{};
        multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.sampleShadingEnable = VK_FALSE;
        multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

        VkPipelineColorBlendAttachmentState colorBlendAttachment{};
        colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        colorBlendAttachment.blendEnable = VK_FALSE;

        VkPipelineColorBlendStateCreateInfo colorBlending{};
        colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlending.logicOpEnable = VK_FALSE;
        colorBlending.logicOp = VK_LOGIC_OP_COPY;
        colorBlending.attachmentCount = 1;
        colorBlending.pAttachments = &colorBlendAttachment;
        colorBlending.blendConstants[0] = 0.0f;
        colorBlending.blendConstants[1] = 0.0f;
        colorBlending.blendConstants[2] = 0.0f;
        colorBlending.blendConstants[3] = 0.0f;

        std::vector<VkDynamicState> dynamicStates = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR
        };

        VkPipelineDynamicStateCreateInfo dynamicState{};
        dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
        dynamicState.pDynamicStates = dynamicStates.data();

		const auto& materialDescriptorSetLayouts = materialAsset->GetDescriptorSetLayouts();
        
		std::vector<VkDescriptorSetLayout> finalDescriptorSetLayouts;

        finalDescriptorSetLayouts.insert(finalDescriptorSetLayouts.end(), m_GlobalDescriptorSetLayouts.begin(), m_GlobalDescriptorSetLayouts.end());
        finalDescriptorSetLayouts.insert(finalDescriptorSetLayouts.end(), materialDescriptorSetLayouts.begin(), materialDescriptorSetLayouts.end());

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(finalDescriptorSetLayouts.size());
        pipelineLayoutInfo.pSetLayouts = finalDescriptorSetLayouts.data();

        VkPipelineLayout pipelineLayout;

        if (vkCreatePipelineLayout(VulkanEngine::GetDevice(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create pipeline layout!");
        }

        VkPipelineDepthStencilStateCreateInfo depthStencil{};
        depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        depthStencil.depthTestEnable = config.depthTest;
        depthStencil.depthWriteEnable = config.depthWrite;
        depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
        depthStencil.depthBoundsTestEnable = VK_FALSE;
        depthStencil.minDepthBounds = 0.0f; // Optional
        depthStencil.maxDepthBounds = 1.0f; // Optional
        depthStencil.stencilTestEnable = VK_FALSE;
        depthStencil.front = {}; // Optional
        depthStencil.back = {}; // Optional

        VkGraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.stageCount = 2;

        pipelineInfo.pStages = shaderStageInfos.data();
        pipelineInfo.pVertexInputState = &vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &inputAssembly;
        pipelineInfo.pViewportState = &viewportState;
        pipelineInfo.pRasterizationState = &rasterizer;
        pipelineInfo.pMultisampleState = &multisampling;
        pipelineInfo.pColorBlendState = &colorBlending;
        pipelineInfo.pDynamicState = &dynamicState;
        pipelineInfo.layout = pipelineLayout;
        pipelineInfo.renderPass = VulkanEngine::GetRenderPass();
        pipelineInfo.subpass = 0;
        pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
        pipelineInfo.pDepthStencilState = &depthStencil;

		VkPipeline pipeline;

        if (vkCreateGraphicsPipelines(VulkanEngine::GetDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create graphics pipeline!");
        }

        return { pipeline, pipelineLayout};
	}

    std::pair<VkPipeline,VkPipelineLayout> GraphicsPipeline::GetPipeline(const std::shared_ptr <MaterialAsset>& materialAsset, const std::shared_ptr<MeshAsset>& meshAsset)
    {
        const auto& vertexBuffer = meshAsset->GetVertexBuffer();
		PipelineKey key = { materialAsset->GetHandle(), vertexBuffer.GetMetaData()};

        if (m_Pipelines.find(key) == m_Pipelines.end())
        {
            m_Pipelines[key] = CreatePipeline(materialAsset,vertexBuffer.GetBindingDescription(),vertexBuffer.GetAttributeDescriptions());
        }

        return m_Pipelines[key];
    }

    void GraphicsPipeline::BindPipeline(const std::shared_ptr<MaterialAsset>& materialAsset, const std::shared_ptr<MeshAsset>& meshAsset)
    {
        auto& let = m_GlobalUniformSetStorage[0].BindingStorage[0].UniformBuffersMapped[VulkanEngine::GetCurrentFrame()];

        static auto startTime = std::chrono::high_resolution_clock::now();

        auto currentTime = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

        glm::mat4 model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        //glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float)Window::WIDTH / (float)Window::HEIGHT, 0.1f, 10.0f);

        proj[1][1] *= -1;

        //model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));

        auto size = sizeof(glm::mat4);

        glm::mat4 data[3] = { model,view,proj };
        memcpy(let, data, size);
        memcpy((uint8_t*)let + size, (uint8_t*)data + size, size * 2);

		materialAsset->UpdateUniformBuffer(VulkanEngine::GetCurrentFrame());

        const auto& [pipeline, pipelineLayout] = GraphicsPipeline::GetPipeline(materialAsset, meshAsset);
		const auto& sets = materialAsset->GetDescriptorSets(VulkanEngine::GetCurrentFrame());

		std::vector<VkDescriptorSet> descriptorSets;

        descriptorSets.insert(descriptorSets.end(), m_GlobalDescriptorSetsChache[VulkanEngine::GetCurrentFrame()].begin(), m_GlobalDescriptorSetsChache[VulkanEngine::GetCurrentFrame()].end());
        descriptorSets.insert(descriptorSets.end(), sets.begin(), sets.end());

        vkCmdBindDescriptorSets(
            VulkanEngine::GetCommandBuffer(),
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            pipelineLayout,
            0,
            descriptorSets.size(),
            descriptorSets.data(),
            0, 
            nullptr);

        vkCmdBindPipeline(VulkanEngine::GetCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
    }   

    void GraphicsPipeline::CleanUp()
    {
        ZoneScoped;
        for (auto& [key, pipeline] : m_Pipelines)
        {
            vkDestroyPipeline(VulkanEngine::GetDevice(), pipeline.first, nullptr);
            vkDestroyPipelineLayout(VulkanEngine::GetDevice(), pipeline.second, nullptr);
        }
        m_Pipelines.clear();

        for (auto& layout : m_GlobalDescriptorSetLayouts)
        {
            vkDestroyDescriptorSetLayout(VulkanEngine::GetDevice(), layout, nullptr);
		}

        for(auto& setStorage : m_GlobalUniformSetStorage)
        {
            for (auto& [bindingIndex, storage] : setStorage.BindingStorage)
            {
                for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
                {
                    vkDestroyBuffer(VulkanEngine::GetDevice(), storage.UniformBuffers[i], nullptr);
                    vkFreeMemory(VulkanEngine::GetDevice(), storage.UniformBuffersMemory[i], nullptr);
                }
            }
            vkDestroyDescriptorSetLayout(VulkanEngine::GetDevice(), setStorage.DescriptorSetLayout, nullptr);
		}

	}
}