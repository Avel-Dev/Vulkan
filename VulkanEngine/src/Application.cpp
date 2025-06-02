#include "Application.h"
#include "Renderer/Renderer.h"

namespace CHIKU
{
	Application::Application()
	{
	}

	void Application::Init()
	{
		ZoneScoped;    // Profile this block
		m_Window.Init();
		m_Engine.Init(m_Window.GetWindow());
		Renderer::s_Instance->Init();
	}

	void Application::Run()
	{
		ZoneScoped;

		while (!m_Window.WindowShouldClose())
		{
			FrameMark;

			m_Window.WindowPoolEvent();
			m_Engine.BeginFrame();
			Renderer::s_Instance->Draw();
			m_Engine.EndFrame();
		}
	}

	void Application::CleanUp()
	{
		ZoneScoped;

		m_Engine.Wait();
		Renderer::s_Instance->CleanUp();
		m_Engine.CleanUp();
	}
}