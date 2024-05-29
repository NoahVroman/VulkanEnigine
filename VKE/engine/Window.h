#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <string>

namespace Engine
{
	class Window
	{
	public:
		Window(const std::string& windowName , int width,int height);
		~Window();

		Window(const Window&) = delete;
		Window& operator=(const Window&) = delete;


		bool ShouldClose() { return glfwWindowShouldClose(m_Window); }

		VkExtent2D GetExtent() { return { static_cast<uint32_t>(m_Width), static_cast<uint32_t>(m_Height) }; }

		bool wasWindowResized() const { return framebufferResized; }
		void resetWindowResizedFlag() { framebufferResized = false; }

		GLFWwindow* GetWindow() const { return m_Window; }


		void CreateWindowSurface(VkInstance instance, VkSurfaceKHR* surface);

	private:

		static void framebufferResizeCallback(GLFWwindow* window, int width, int height);
		void InitWindow();

		int m_Width;
		int m_Height;

		bool framebufferResized = false;

		std::string m_WindowName;
		GLFWwindow* m_Window;
	};
}
