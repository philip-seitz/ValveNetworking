#pragma once
#include <ValveNetworking.h>

class ClientApp: public ValveNetworking::Application
{
public:
	ClientApp();
	~ClientApp();

	void Run();

private:
	SteamNetworkingIPAddr m_ServerAddress;		// Server IP address
	ValveNetworking::Client m_Client;
};

ValveNetworking::Application* ValveNetworking::CreateApplication()
{
	return new ClientApp;
}