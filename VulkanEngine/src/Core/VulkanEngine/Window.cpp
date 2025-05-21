#include "Window.h"

namespace CHIKU
{
	uint32_t Window::WIDTH = 1280;
	uint32_t Window::HEIGHT = 720;

	void Window::Init()
	{
		InitWindow();
	}

	void Window::InitWindow()
	{
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
		m_Window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
	}
}