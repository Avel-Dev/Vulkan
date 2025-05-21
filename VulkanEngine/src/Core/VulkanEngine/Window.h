#pragma once
#include "VulkanHeader.h"

namespace CHIKU
{
	class Window
	{
	public:
		static uint32_t WIDTH;
		static uint32_t HEIGHT;

	public:
		void Init();

		inline GLFWwindow* GetWindow() { return m_Window; }
		inline bool WindowShouldClose() const { return glfwWindowShouldClose(m_Window); }
		inline void WindowPoolEvent() const { glfwPollEvents(); }

	private:
		void InitWindow();

	private:
		GLFWwindow* m_Window;
	};
}