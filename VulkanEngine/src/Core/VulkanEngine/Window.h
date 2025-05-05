#pragma once
#include "VulkanHeader.h"

namespace CHIKU
{
	class Window
	{
	public:
		void Init();
		GLFWwindow* GetWindow() { return m_Window; }

	private:
		void InitWindow();

	private:
		GLFWwindow* m_Window;
		uint32_t WIDTH = 800;
		uint32_t HEIGHT = 600;
	};
}