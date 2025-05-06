#pragma once
#include "VulkanHeader.h"

namespace CHIKU
{
	class Window
	{
	public:
		void Init();

		inline GLFWwindow* GetWindow() { return m_Window; }
		inline bool WindowShouldClose() const { return glfwWindowShouldClose(m_Window); }
		inline void WindowPoolEvent() const { glfwPollEvents(); }

	private:
		void InitWindow();

	private:
		GLFWwindow* m_Window;
		uint32_t WIDTH = 800;
		uint32_t HEIGHT = 600;
	};
}