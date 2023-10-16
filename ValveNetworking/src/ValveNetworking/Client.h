#pragma once
#include <steam/steamnetworkingsockets.h>
#include <steam/isteamnetworkingutils.h>
#include <thread>
#include <mutex>
#include <queue>
#include <string>
#include <stdarg.h>
#include "Core.h"

namespace ValveNetworking
{
	class VN_API Client
	{
	public:
		void Run(const SteamNetworkingIPAddr& serverAddr);
		void StopProcess(int rc);
	private:
		bool m_Running;
		HSteamNetConnection m_Connection;
		ISteamNetworkingSockets* m_Interface;

		// Input Threading 
		std::thread* m_InputThread;
		std::mutex m_InputMutex;
		std::queue<std::string> m_InputQueue;
		SteamNetworkingMicroseconds m_LoggedTime;

		void HandleIncomingMessages();
		void HandleLocalUserInput();
		void OnNetConnectionStatusChanged(SteamNetConnectionStatusChangedCallback_t* pInfo);
		static void NetConnectionStatusChangedCallback(SteamNetConnectionStatusChangedCallback_t* pInfo);
		void HandleConnectionStateChanges();
		void LocalUserInput_Init();
		bool LocalUserInput_GetNext(std::string& result);
		void InitSteamDatagramConnectionSockets();
		void ShutdownSteamDatagramConnectionSockets();
		void PrintfTime(const char* msg, ...);
		// should probably add FatalError() back '-' 
	};
}