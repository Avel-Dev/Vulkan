#include "Application.h"

namespace CHIKU
{
	void Application::Init()
	{
		m_Engine.Init();
	}

	void Application::Run()
	{
	}

	void Application::CleanUp()
	{
		m_Engine.CleanUp();
	}
}