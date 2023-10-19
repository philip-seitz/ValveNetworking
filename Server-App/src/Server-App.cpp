#define _CRT_SECURE_NO_WARNINGS
#define STEAMNETWORKINGSOCKETS_OPENSOURCE
#include "Server-App.h"
#include <ValveNetworking.h>
#include <chrono>
#include <thread>
#include <mutex>
#include <queue>
#include <iostream>

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

	// m_Server.StartListening // init and start of listening
	// while m_Server.m_Running
		// message / input polling	   (on event)	-> i.e. /move_r -> update pos to the right; normal msgs aswell...


	m_Server.Run((uint16)DEFAULT_SERVER_PORT);
}
