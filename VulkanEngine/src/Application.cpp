#include "Application.h"

namespace CHIKU
{
	Application::Application()
	{
		model = nullptr;
	}

	void Application::Init()
	{
		m_Window.Init();
		m_Engine.Init(m_Window.GetWindow());
		m_Renderer.Init();

		model = new Model();
		model->LoadMesh("models/viking_room.obj");
		pipeline.CreatePipeline(*model, VulkanEngine::GetRenderPass());
	}

	void Application::Run()
	{
		while (!m_Window.WindowShouldClose())
		{
			m_Window.WindowPoolEvent();
			m_Renderer.BeginFrame();

			m_Renderer.BindGraphicsPipeline(pipeline);
			pipeline.Draw(VulkanEngine::GetCommandBuffer());

			m_Renderer.EndFrame();
		}
	}

	void Application::CleanUp()
	{
		m_Engine.Wait();
		delete(model);
		pipeline.CleanUp();
		m_Renderer.Shutdown();
		m_Engine.CleanUp();
	}
}