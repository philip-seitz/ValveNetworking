#pragma once
#define STEAMNETWORKINGSOCKETS_OPENSOURCE
#include <steam/steamnetworkingsockets.h>
#include <steam/isteamnetworkingutils.h>
#ifndef STEAMNETWORKINGSOCKETS_OPENSOURCE
#include <steam/steam_api.h>
#endif
#include <map>
#include <iostream>
#include <thread>
#include <mutex>
#include <queue>
#include <string>
#include <stdarg.h>
#include "Core.h"


namespace ValveNetworking {
	class VN_API Server
	{
	public:
		bool IsRunning();
		void Run(uint16 port);
		void StartListening(uint16 port);
		void PollConnection();
		void PollInput();
		void Shutdown();
		void StopProcess(int rc);

	private:
		bool m_Running;
		HSteamListenSocket m_ListenSocket;
		HSteamNetPollGroup m_PollGroup;
		ISteamNetworkingSockets* m_Interface;

		// Input Threading 
		std::thread* m_InputThread;
		std::mutex m_InputMutex;
		std::queue<std::string> m_InputQueue;
		SteamNetworkingMicroseconds m_LoggedTime;

		struct ClientData
		{
			std::string nickname;
			// int age;
			// int salary;
		};
		std::map<HSteamNetConnection, ClientData> m_Clients;
		uint16 m_Port;			// port number range {0...65535}

		void OnNetConnectionStatusChanged(SteamNetConnectionStatusChangedCallback_t* pInfo);
		static void NetConnectionStatusChangedCallback(SteamNetConnectionStatusChangedCallback_t* pInfo);
		void SendStringToClient(HSteamNetConnection conn, const char* str);
		void SendStringToAllClients(const char* str, HSteamNetConnection except = k_HSteamNetConnection_Invalid);
		void SendFloatToClient(HSteamNetConnection conn, uint8 nmbr);
		void HandleIncomingMessages();
		void HandleLocalUserInput();
		bool LocalUserInput_GetNext(std::string& result);
		void LocalUserInput_Init();
		void InitSteamDatagramConnectionSockets();
		void ShutdownSteamDatagramConnectionSockets();
		void PrintfTime(const char* msg, ...);

		void SetClientNickname(HSteamNetConnection hConn, const char* nickname);
		void HandleConnectionStateChanges();
	};
}