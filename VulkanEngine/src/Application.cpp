#include "Application.h"
#include "Renderer/Renderer.h"
#include "VulkanEngine/Assets/AssetManager.h"
#include "Utils/ShaderUtils.h"

namespace CHIKU
{
	Application::Application()
	{
	}

	void Application::Init()
	{
		ZoneScoped;    // Profile this block
	
		Utils::LoadSPIRV(SOURCE_DIR + "src/VulkanEngine/Shader/unlit.vert.spv");

		m_Window.Init();
		m_Engine.Init(m_Window.GetWindow());
		Renderer::Init();
		AssetManager::Init();
		AssetManager::AddModel("Models/Box/Box.gltf");
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
			Renderer::Draw();
			m_Engine.EndFrame();
		}
	}

	void Application::CleanUp()
	{
		ZoneScoped;

		m_Engine.Wait();
		Renderer::CleanUp();
		AssetManager::CleanUp();
		m_Engine.CleanUp();
	}
}