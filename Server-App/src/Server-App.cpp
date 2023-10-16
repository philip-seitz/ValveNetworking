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
	m_Server.Run((uint16)DEFAULT_SERVER_PORT);
}
