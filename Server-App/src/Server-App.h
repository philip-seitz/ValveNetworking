#pragma once
#include <ValveNetworking.h>

class ServerApp : public ValveNetworking::Application
{
public:
	ServerApp();
	~ServerApp();
	virtual void Run();


private:

	SteamNetworkingIPAddr m_ServerAddress;			// Server IP address
	ValveNetworking::Server m_Server;
};

ValveNetworking::Application* ValveNetworking::CreateApplication()
{
	return new ServerApp();
}