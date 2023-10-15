#define _CRT_SECURE_NO_WARNINGS
#define STEAMNETWORKINGSOCKETS_OPENSOURCE
#include "Server-App.h"
#include "ValveNetworking.h"
#include <chrono>
#include <thread>
#include <mutex>
#include <queue>

#ifdef _WIN32
#include <windows.h> // Ug, for NukeProcess -- see below
#else
#include <unistd.h>
#include <signal.h>
#endif

// TODO: clean up and maybe move to lib
static void NukeProcess(int rc)
{
#ifdef _WIN32
	ExitProcess(rc);
#endif
}

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
	NukeProcess(0);
}

//int main()
//{
//	SteamNetworkingIPAddr addrServer;	// Server IP address
//	addrServer.Clear();					// server IP and port set to zero
//
//	ValveNetworking::Server server;
//	server.Run((uint16)DEFAULT_SERVER_PORT);
//
//	NukeProcess(0);
//}