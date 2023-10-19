#pragma once
#include <steam/steamnetworkingsockets.h>
#include <steam/isteamnetworkingutils.h>
#include <thread>
#include <mutex>
#include <queue>
#include <string>
#include <stdarg.h>
#include "Core.h"

#include "glm/common.hpp"
#include "glm/glm.hpp"

namespace ValveNetworking
{
	enum MovementDirection
	{
		MOVE_UP = 1, MOVE_DOWN, MOVE_LEFT, MOVE_RIGHT
	};

	enum ConnectionStatus
	{
		CON_PENDING = 1, CON_FAILED, CON_SUCCESS
	};
	
	class VN_API Client
	{
	public:
		bool IsRunning();
		ConnectionStatus GetConnectionStatus();
		void Connect(const char* serverIP, const uint16 port);
		void PollConnection();
		void PollInput();
		void SendMovementInfo(MovementDirection dir);
		void Shutdown();
		void Run(const SteamNetworkingIPAddr& serverAddr);
		void StopProcess(int rc);
		std::queue<MovementDirection> m_MovementQueue;
	private:
		bool m_Running;
		HSteamNetConnection m_Connection;
		ISteamNetworkingSockets* m_Interface;
		ConnectionStatus m_ConnectionStatus;

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