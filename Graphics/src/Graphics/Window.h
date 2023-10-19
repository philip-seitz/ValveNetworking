#pragma once
// basic windowing class 
// glew lib is  statically linked (Macro disables dll)

#include <iostream>
#include "GL\glew.h"
#include "GLFW\glfw3.h"	

namespace Graphics
{
	class Window
	{
	public:
		int m_Height;
		int m_Width;
		Window(const int& width, const int& height, const char* title) :
			m_AllRunning(true),
			m_InstWindow(nullptr),
			m_Height(height),
			m_Width(width)
		{
			if (!glfwInit())
				m_AllRunning = false;
			else
			{
				int w = width;
				int h = height;
				ScaleWindowSize(w, h);					// resolution based scaling
				// set OpenGL Version 3.3
				glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
				glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
				m_InstWindow = glfwCreateWindow(w, h, title, NULL, NULL);

				glViewport(0, 0, w, h);
				m_Width = w;
				m_Height = h;
			}
		}
		~Window()
		{
			if (m_InstWindow)
				glfwTerminate();
		}

		bool Init()
		{
			if (!m_AllRunning)
				return false;
			else
			{
				if (!m_InstWindow)
				{
					glfwTerminate();
					m_AllRunning = false;
				}
				else
				{
					glfwMakeContextCurrent(m_InstWindow);
					if (glewInit() != GLEW_OK)
						std::cout << "error" << std::endl;

					std::cout << glGetString(GL_VERSION) << std::endl;
					m_AllRunning = true;
				}
			}
			return m_AllRunning;
		}

		GLFWwindow* GetWindow()
		{
			return m_InstWindow;
		}

	private:
		bool m_AllRunning;
		GLFWwindow* m_InstWindow;
		void ScaleWindowSize(int& w, int& h)
		{
			int scale = 1;
			const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
			if (mode->width > 1920)
				scale = 2;

			w *= scale;
			h *= scale;
		}
	};
}