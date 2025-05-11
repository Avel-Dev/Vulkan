#include "Renderer.h"
#include "VulkanEngine/VulkanEngine.h"

namespace CHIKU
{
	void Renderer::Init()
	{
	}
		
	void Renderer::Shutdown()
	{
	}

	void Renderer::BeginFrame()
	{
		VulkanEngine::BeginFrame();
	}

	void Renderer::EndFrame()
	{
		VulkanEngine::EndFrame();
	}

	void Renderer::BeginScene()
	{
	}

	void Renderer::EndScene()
	{
	}

	void Renderer::BindGraphicsPipeline(GraphicsPipeline pipeline)
	{
		pipeline.Bind(VulkanEngine::GetCommandBuffer());
	}

	void Renderer::DrawMesh(const Mesh& mesh, const Material& material, const glm::mat4& transform)
	{

	}

	void Renderer::DrawModel(const Model& model, const glm::mat4& transform)
	{

	}

	void Renderer::BeginShadowPass()
	{
	}

	void Renderer::EndShadowPass()
	{
	}

	void Renderer::BeginPostProcessing()
	{
	}

	void Renderer::EndPostProcessing()
	{
	}
}