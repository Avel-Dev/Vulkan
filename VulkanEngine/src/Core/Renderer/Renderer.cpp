#include "Renderer.h"
#include "VulkanEngine/VulkanEngine.h"

namespace CHIKU
{
	void Renderer::Init()
	{
		m_Model = new Model();
		m_Model->LoadMesh("models/viking_room.obj");
	}
		
	void Renderer::Shutdown()
	{
		delete(m_Model);
	}

	void Renderer::BeginFrame()
	{
		VulkanEngine::s_Instance->BeginFrame();
	}

	void Renderer::EndFrame()
	{
		VulkanEngine::s_Instance->EndFrame();
	}

	void Renderer::BeginScene()
	{
	}

	void Renderer::EndScene()
	{
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