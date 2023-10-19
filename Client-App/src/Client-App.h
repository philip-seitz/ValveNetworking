#pragma once
#include <ValveNetworking.h>
#include <Graphics.h>
#include <GL/glew.h>
#include <glm/glm.hpp>


class ClientApp: public ValveNetworking::Application
{
public:
	ClientApp();
	~ClientApp();
	void Run();

private:
	ValveNetworking::Client m_Client;
	Graphics::Window m_Window;
	glm::vec4 m_ClearColor;

	// variables for quad
	Graphics::Quad m_Quad;
	Graphics::Camera m_Camera2D;

	// grid
	Graphics::GridInstance m_Grid_xy;
};

ValveNetworking::Application* ValveNetworking::CreateApplication()
{
	return new ClientApp;
}