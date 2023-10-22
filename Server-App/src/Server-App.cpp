#define _CRT_SECURE_NO_WARNINGS
#define STEAMNETWORKINGSOCKETS_OPENSOURCE
#include "Server-App.h"
#include <ValveNetworking.h>
#include <chrono>
#include <thread>
#include <mutex>
#include <queue>
#include <iostream>
#include <chrono>

const uint16 DEFAULT_SERVER_PORT = 27020;


ServerApp::ServerApp()
{
	m_ServerAddress.Clear();
}

ServerApp::~ServerApp() 
{

}

void ServerApp::Run()
{
	std::chrono::time_point<std::chrono::system_clock> currentTime, lastTime;
	currentTime = std::chrono::system_clock::now();
	lastTime = currentTime;
	long long timer = 0;

	// m_Server.StartListening // init and start of listening
	// while m_Server.m_Running
		// message / input polling	   (on event)	-> i.e. /move_r -> update pos to the right; normal msgs aswell...

	m_Server.StartListening(DEFAULT_SERVER_PORT);

	
	while (m_Server.IsRunning())
	{
		currentTime = std::chrono::system_clock::now();
		std::chrono::duration<float>dt = currentTime - lastTime;
		long long  temp = dt.count() * 1000;
		timer += temp;		// ms
		
		
		if (timer >= 10)
		{
			m_Server.PollConnection();
			m_Server.PollInput();
			timer = 0;
			lastTime = currentTime;
		}
		
	}

	m_Server.Shutdown();

	// m_Server.Run((uint16)DEFAULT_SERVER_PORT);
}
