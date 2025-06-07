#include "Application.h"
#include "Renderer/Renderer.h"
#include "VulkanEngine/Assets/AssetManager.h"
#include "VulkanEngine/SceneManager.h"

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

		AssetManager::AddAsset(AssetType::Mesh,"models/viking_room.obj");
	}

	void Application::Run()
	{
		ZoneScoped;    // Profile this block
		while (!m_Window.WindowShouldClose())
		{
			FrameMark;
		}
	}

	//Renderer Thread Process
	void Application::Render()
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