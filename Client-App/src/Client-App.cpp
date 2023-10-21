#define _CRT_SECURE_NO_WARNINGS
#define STEAMNETWORKINGSOCKETS_OPENSOURCE
#include <ValveNetworking.h>
#include <Graphics.h>
#include "Client-App.h"

const int window_width = 800;
const int window_height = 400;

const uint16 DEFAULT_SERVER_PORT = 27020;
const char* serverIP = "192.168.178.68";

struct RendererData
{
	Graphics::Model quad;
	Graphics::Grid grid;
	Graphics::Shader* shader = nullptr;
	Graphics::Shader* grid_shader = nullptr;
};

RendererData sData;

ClientApp::ClientApp():
	m_Window(window_width, window_height, "Client-App"),
	m_Quad(false),											// no textures
	m_Camera2D(false)										// orthographic
{
	m_Grid_xy.m_position.z = -4.0f;
	// init windowing and open gl context 
	m_ClearColor = { 0.2f, 0.2f, 0.2f, 1.0f };
	glfwHideWindow(m_Window.GetWindow());
}

ClientApp::~ClientApp()
{

}

void ClientApp::Run()
{

	m_Client.Connect(serverIP, DEFAULT_SERVER_PORT);		// init and connection
	if (m_Client.IsRunning())
	{
		while (1)											// wait for continuation for connection result
		{
			m_Client.PollConnection();
			// client init successful -> IsRunning();   Connection to Host successful -> CON_SUCCESS
			if (m_Client.GetConnectionStatus() == ValveNetworking::CON_SUCCESS)		// window setup if connection was successful
			{
				if (!m_Window.Init())
					std::exit(0);
				glfwShowWindow(m_Window.GetWindow());
				sData.quad.InitQuad(3.0f, { 0.0f, 0.0f, -0.5f });
				sData.grid.InitGridxy();
				sData.shader = new Graphics::Shader("../Graphics/res/shader/simple_shader.glsl");
				sData.grid_shader = new Graphics::Shader("../Graphics/res/shader/grid_shader.glsl");
				break;
			}
			else if (m_Client.GetConnectionStatus() == ValveNetworking::CON_FAILED)		// don't create window if connection failed 
			{
				// if client is not running 
				break;
			}
		}
	}
	
	float currentTime = glfwGetTime();
	float lastTime = currentTime;
	float timer = 0.0f;

	static bool w_pressed = false;
	static bool s_pressed = false;

	while (m_Client.IsRunning() && !glfwWindowShouldClose(m_Window.GetWindow()))
	{
		currentTime = glfwGetTime();
		float dt = currentTime - lastTime;
		timer += dt;
		if (timer >= 0.01f)
		{
			// if 10ms frequency
			m_Client.PollConnection();							// save message from server (for movement etc.)
			m_Client.PollInput();								// Send messages based on input
			// update position for example (on update)			-> use saved mesage to update

			timer = 0.0f;
		}
		lastTime = currentTime;

		// update position every frame

		bool w_handled = false;
		bool a_handled = false;
		bool s_handled = false;
		bool d_handled = false;

		while (!m_Client.m_MovementQueue.empty())
		{

			if (m_Client.m_MovementQueue.front() == ValveNetworking::MOVE_UP)
			{
				if (!w_handled)
				{
					m_Quad.m_Instance.position.y += 0.4f;
					w_handled = true;
				}
			}
			else if (m_Client.m_MovementQueue.front() == ValveNetworking::MOVE_DOWN)
			{
				if (!s_handled)
				{
					m_Quad.m_Instance.position.y -= 0.4f;
					s_handled = true;
				}
			}
			m_Client.m_MovementQueue.pop();		// empty queue iteratively
		}
		glEnable(GL_DEPTH_TEST);
		// render stuff if necessary or set to render			-> render quad with pos from msg
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

		// m_Camera2D.ManageInputs(m_Window, dt);

		// manage inputs here 
		// movement
		if(glfwGetKey(m_Window.GetWindow(), GLFW_KEY_W) == GLFW_PRESS)
		{
			w_pressed = true;
			// m_Quad.m_Instance.position.y += dt * 3.0f;
		}
		if (w_pressed && glfwGetKey(m_Window.GetWindow(), GLFW_KEY_W) == GLFW_RELEASE)
		{
			w_pressed = false;
			m_Client.SendMovementInfo(ValveNetworking::MovementDirection::MOVE_UP);
		}
		if (glfwGetKey(m_Window.GetWindow(), GLFW_KEY_S) == GLFW_PRESS)
		{
			s_pressed = true;
		}
		if (s_pressed && glfwGetKey(m_Window.GetWindow(), GLFW_KEY_S) == GLFW_RELEASE)
		{
			s_pressed = false;
			m_Client.SendMovementInfo(ValveNetworking::MovementDirection::MOVE_DOWN);
		}


		// draw grid
		sData.grid_shader->SetUniformMat4f("u_View", m_Camera2D.LookAtMatrix());
		sData.grid_shader->SetUniformMat4f("u_Projection", m_Camera2D.ProjectionMatrix());
		m_Grid_xy.Draw(sData.grid, sData.grid_shader);
		m_Camera2D.SetPosition({ m_Quad.m_Instance.position.x, m_Quad.m_Instance.position.y, m_Camera2D.Position().z});					// move camera with quad
		sData.shader->SetUniformMat4f("u_View", m_Camera2D.LookAtMatrix());
		sData.shader->SetUniformMat4f("u_Projection", m_Camera2D.ProjectionMatrix());
		m_Quad.Draw(sData.shader, &sData.quad);

		glfwSwapBuffers(m_Window.GetWindow());
		glfwPollEvents();
	}

	// shutdown (connection and rendering)
	m_Client.Shutdown();
	delete sData.shader;
	delete sData.grid_shader;
}
