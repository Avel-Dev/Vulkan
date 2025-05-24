#include "GraphicsPipeline.h"
#include "VulkanEngine/VulkanEngine.h"
#include "BufferUtils.h"
#include "Shader.h"
#include <chrono>
#include <array>


namespace CHIKU
{
    std::unordered_map<PipelineKey, GraphicsPipeline::Pipeline> GraphicsPipeline::sm_GrphicsPipeline;

	void GraphicsPipeline::Init()
	{
	}

    void GraphicsPipeline::Bind(const Material& material, const VertexBuffer& vertexbuffer)
    {
        PipelineKey key = {
           material.GetShaderID(),
           vertexbuffer.GetBufferLayout(),
           material.GetMaterialType()
        };

        GetOrCreateGraphicsPipeline(key, material, vertexbuffer);

        vkCmdBindPipeline(VulkanEngine::GetCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, sm_GrphicsPipeline.at(key).GraphicsPipeline);

        material.Bind(sm_GrphicsPipeline.at(key).PipelineLayout);
        vertexbuffer.Bind();
    }

    void GraphicsPipeline::CleanUp()
    {
        for (auto i : sm_GrphicsPipeline)
        {
            vkDestroyPipeline(VulkanEngine::GetDevice(), i.second.GraphicsPipeline, nullptr);
            vkDestroyPipelineLayout(VulkanEngine::GetDevice(), i.second.PipelineLayout, nullptr);
        }

        sm_GrphicsPipeline.clear();
    }

    GraphicsPipeline::Pipeline GraphicsPipeline::GetOrCreateGraphicsPipeline(PipelineKey key, const Material& material, const VertexBuffer& vertexBuffer)
    {
        if (sm_GrphicsPipeline.find(key) == sm_GrphicsPipeline.end())
        {
            sm_GrphicsPipeline[key] = CreateGraphicsPipeline(
                ShaderManager::GetShaderStages(material.GetShaderID()).data(),
                vertexBuffer.GetBufferDescription(), 
                material.GetDescriptorLayout()
            );
        }

        return sm_GrphicsPipeline.at(key);
    }

    GraphicsPipeline::Pipeline GraphicsPipeline::CreateGraphicsPipeline(const VkPipelineShaderStageCreateInfo* pipelineStages, VertexBuffer::VertexInputDescription description, VkDescriptorSetLayout layout)
	{
        VkPipelineLayout pipelineLayout;
        VkPipeline graphicsPipeline;

        VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

        vertexInputInfo.vertexBindingDescriptionCount = 1;
        vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(description.AttributeDescription.size());
        vertexInputInfo.pVertexBindingDescriptions = &description.BindingDescription;
        vertexInputInfo.pVertexAttributeDescriptions = description.AttributeDescription.data();

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

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 1;

        pipelineLayoutInfo.pSetLayouts = &layout;

        if (vkCreatePipelineLayout(VulkanEngine::GetDevice(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create pipeline layout!");
        }

        VkPipelineDepthStencilStateCreateInfo depthStencil{};
        depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        depthStencil.depthTestEnable = VK_TRUE;
        depthStencil.depthWriteEnable = VK_TRUE;
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

        pipelineInfo.pStages = pipelineStages;
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

        if (vkCreateGraphicsPipelines(VulkanEngine::GetDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create graphics pipeline!");
        }

        return {
            pipelineLayout,
            graphicsPipeline
        };
	}
}
