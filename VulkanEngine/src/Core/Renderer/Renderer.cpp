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
}