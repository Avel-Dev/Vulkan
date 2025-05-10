#include "Application.h"

namespace CHIKU
{
	Application::Application()
	{
	}

	void Application::Init()
	{
		m_Window.Init();
		m_Engine.Init(m_Window.GetWindow());
		m_Renderer.Init();
	}

	void Application::Run()
	{
		while (!m_Window.WindowShouldClose())
		{
			m_Window.WindowPoolEvent();

			m_Renderer.BeginFrame();
			m_Renderer.BeginScene();
			m_Renderer.EndScene();
			m_Renderer.EndFrame();
		}
	}

	void Application::CleanUp()
	{
		m_Renderer.Shutdown();
		m_Engine.CleanUp();
	}
}