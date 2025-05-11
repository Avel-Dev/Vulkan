#pragma once
#include "VulkanHeader.h"
#include "Model.h"

namespace CHIKU
{
	struct PipelineState
	{
		const Model& model;
		VkPipelineLayout m_PipelineLayout;
		VkPipeline m_GraphicsPipeline;
	};

	class GraphicsPipeline 
	{
	public:
		~GraphicsPipeline();

		void CreatePipeline(const Model& model, VkRenderPass renderpass);
		void Bind(VkCommandBuffer commandBuffer);
		void Draw(VkCommandBuffer commandBuffer) const;
		void CleanUp();
	private:
		std::vector<PipelineState> m_State;
	};
}