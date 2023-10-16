#define _CRT_SECURE_NO_WARNINGS
#define STEAMNETWORKINGSOCKETS_OPENSOURCE
#include "Client-App.h"
#include <ValveNetworking.h>

const uint16 DEFAULT_SERVER_PORT = 27020;
const char* serverIP = "2001:9e8:3852:f200:71ff:6065:2413:53e6";
ClientApp::ClientApp()
{
	m_ServerAddress.Clear();
	m_ServerAddress.ParseString(serverIP);
	m_ServerAddress.m_port = DEFAULT_SERVER_PORT;
}

ClientApp::~ClientApp()
{

}



void ClientApp::Run()
{
	m_Client.Run(m_ServerAddress);
}
