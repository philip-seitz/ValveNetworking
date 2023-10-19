#define _CRT_SECURE_NO_WARNINGS
#include "Client.h"
#include <thread>
#include <mutex>
#include <queue>
#include <string>
#include <stdarg.h>
#include <windows.h>

#include <glm/glm.hpp>
#include <glm/common.hpp>
#include "NetworkingUtils.h"

namespace ValveNetworking
{
	Client* s_CallbackInstance = nullptr;

	bool Client::IsRunning()
	{
		return m_Running;
	}

	ConnectionStatus Client::GetConnectionStatus()
	{
		return m_ConnectionStatus;
	}

	void Client::Connect(const char* serverIP, const uint16 port)
	{
		m_Running = false;
		InitSteamDatagramConnectionSockets();		// init API
		LocalUserInput_Init();						// setup user input thread (~non blocking)
		m_Interface = SteamNetworkingSockets();

		// setup ip and port
		SteamNetworkingIPAddr serverAddr;			
		serverAddr.Clear();
		serverAddr.ParseString(serverIP);
		serverAddr.m_port = port;

		// Start connecting
		char address[SteamNetworkingIPAddr::k_cchMaxString];
		serverAddr.ToString(address, sizeof(address), true);						// write IP to address for print
		PrintfTime("Connecting to chat server at %s", address);
		SteamNetworkingConfigValue_t options;
		options.SetPtr(k_ESteamNetworkingConfig_Callback_ConnectionStatusChanged, (void*)NetConnectionStatusChangedCallback);
		m_Connection = m_Interface->ConnectByIPAddress(serverAddr, 1, &options);	// try IP to connect
		if (m_Connection == k_HSteamNetConnection_Invalid)
		{
			// Failed.
			PrintfTime("Failed to create connection");
		}
		else
		{
			// Client init was successful, it is now running.
			m_Running = true;
		}
	}

	void Client::PollConnection()
	{
		HandleIncomingMessages();												// poll messages coming over connection (+print)
		HandleConnectionStateChanges();											// print messages depending on connection/dcs etc.
	}

	void Client::PollInput()
	{
		HandleLocalUserInput();													// poll user messages and send them over connection
	}
	// message includes position of object and the commanded direction
	
	void Client::SendMovementInfo(MovementDirection dir)
	{
		switch (dir)
		{
			case MOVE_UP:
			{
				std::string msg = "MOVE_UP";
				float temp = 33.0f;
				m_Interface->SendMessageToConnection(m_Connection, msg.c_str(), uint32(msg.length()), k_nSteamNetworkingSend_Reliable, nullptr);
				break;
			}
		}
	}

	void Client::Shutdown()
	{
		ShutdownSteamDatagramConnectionSockets();
		StopProcess(0);
	}

	void Client::Run(const SteamNetworkingIPAddr& serverAddr)
	{
		//m_Running = false;
		//InitSteamDatagramConnectionSockets();			// init API
		//LocalUserInput_Init();						// setup user input thread (~non blocking)

		//m_Interface = SteamNetworkingSockets();

		//// Start connecting
		//char address[SteamNetworkingIPAddr::k_cchMaxString];
		//serverAddr.ToString(address, sizeof(address), true);						// write IP to szAddr
		//PrintfTime("Connecting to chat server at %s", address);
		//SteamNetworkingConfigValue_t options;
		//options.SetPtr(k_ESteamNetworkingConfig_Callback_ConnectionStatusChanged, (void*)NetConnectionStatusChangedCallback);
		//m_Connection = m_Interface->ConnectByIPAddress(serverAddr, 1, &options);	// try IP to connect
		//if (m_Connection == k_HSteamNetConnection_Invalid)
		//	PrintfTime("Failed to create connection");

		while (!m_Running)
		{
			HandleIncomingMessages();												// poll messages coming over connection (+print)
			HandleConnectionStateChanges();											// print messages depending on connection/dcs etc.
			HandleLocalUserInput();													// poll user messages and send them over connection
			std::this_thread::sleep_for(std::chrono::milliseconds(10));				// reduce polling frequency to lower load
		}
		ShutdownSteamDatagramConnectionSockets();
		StopProcess(0);
	}

	void Client::StopProcess(int rc)
	{
#ifdef _WIN32
		ExitProcess(rc);
#endif
	}

	void Client::HandleIncomingMessages()
	{
		while (m_Running)
		{
			ISteamNetworkingMessage* pIncomingMsg = nullptr;
			int numMsgs = m_Interface->ReceiveMessagesOnConnection(m_Connection, &pIncomingMsg, 1);
			if (numMsgs == 0)
				break;
			if (numMsgs < 0)
				PrintfTime("Error checking for messages");


			std::string sCmd;
			sCmd.assign((const char*)pIncomingMsg->m_pData, pIncomingMsg->m_cbSize);
			const char* cmd = sCmd.c_str();		// write message to sCmd (string)


			if (strcmp(cmd, "MOVE_UP") == 0)
			{
				m_MovementQueue.push(MOVE_UP);
				// We don't need this anymore.
				pIncomingMsg->Release();
				break;
			}

			// Just echo anything we get from the server
			fwrite(pIncomingMsg->m_pData, 1, pIncomingMsg->m_cbSize, stdout);
			fputc('\n', stdout);

			// We don't need this anymore.
			pIncomingMsg->Release();
		}
	}

	void Client::HandleLocalUserInput()
	{
		std::string cmd;
		while (m_Running && LocalUserInput_GetNext(cmd))		// read in new message (cmd) if any return true
		{
			// Check for known commands
			if (strcmp(cmd.c_str(), "/quit") == 0)
			{
				m_Running = false;
				PrintfTime("Disconnecting from chat server");

				// Close the connection gracefully.
				// We use linger mode to ask for any remaining reliable data
				// to be flushed out.  But remember this is an application
				// protocol on UDP.  See ShutdownSteamDatagramConnectionSockets
				m_Interface->CloseConnection(m_Connection, 0, "Goodbye", true);
				break;
			}
			// Anything else (not quit), just send it to the server and let them parse it
			m_Interface->SendMessageToConnection(m_Connection, cmd.c_str(), (uint32)cmd.length(), k_nSteamNetworkingSend_Reliable, nullptr);
		}
	}


	void Client::OnNetConnectionStatusChanged(SteamNetConnectionStatusChangedCallback_t* pInfo)
	{

		// What's the state of the connection?
		switch (pInfo->m_info.m_eState)	// current connection info
		{
		case k_ESteamNetworkingConnectionState_None:
			// NOTE: We will get callbacks here when we destroy connections.  You can ignore these.
			break;

		case k_ESteamNetworkingConnectionState_ClosedByPeer:
		case k_ESteamNetworkingConnectionState_ProblemDetectedLocally:	// detected problem currently
		{
			m_Running = false;

			// Print an appropriate message
			if (pInfo->m_eOldState == k_ESteamNetworkingConnectionState_Connecting)	// m_eOldState info of last state
			{
				// Note: we could distinguish between a timeout, a rejected connection,
				// or some other transport problem. (connection failed)
				PrintfTime("Connection to host failed.  (%s)", pInfo->m_info.m_szEndDebug);
				m_ConnectionStatus = CON_FAILED;
			}
			else if (pInfo->m_info.m_eState == k_ESteamNetworkingConnectionState_ProblemDetectedLocally)
			{
				PrintfTime("Lost connection to host.  (%s)", pInfo->m_info.m_szEndDebug);
			}
			else
			{
				// NOTE: We could check the reason code for a normal disconnection
				PrintfTime("Host disconnected.  (%s)", pInfo->m_info.m_szEndDebug);
			}

			// Clean up the connection
			m_Interface->CloseConnection(pInfo->m_hConn, 0, nullptr, false);
			m_Connection = k_HSteamNetConnection_Invalid;
			break;
		}

		case k_ESteamNetworkingConnectionState_Connecting:
			m_ConnectionStatus = CON_PENDING;
			break;

		case k_ESteamNetworkingConnectionState_Connected:
			PrintfTime("Connected to server OK");
			m_ConnectionStatus = CON_SUCCESS;
			break;

		default:
			// Silences -Wswitch
			break;
		}
	}

	void Client::NetConnectionStatusChangedCallback(SteamNetConnectionStatusChangedCallback_t* pInfo)
	{
		s_CallbackInstance->OnNetConnectionStatusChanged(pInfo);
	}

	void Client::HandleConnectionStateChanges()
	{
		s_CallbackInstance = this;
		m_Interface->RunCallbacks();
	}

	void Client::LocalUserInput_Init()
	{
		m_InputThread = new std::thread([this]()
		{
			// thread code defined via lambda function
			while (m_Running)
			{
				char szLine[4000];
				if (!fgets(szLine, sizeof(szLine), stdin))		// write user input (console) to szLine 
				{
					// Well, you would hope that you could close the handle
					// from the other thread to trigger this.  Nope.
					if (!m_Running)
						return;
					m_Running = false;
					PrintfTime("Failed to read on stdin, quitting\n");
					break;		// end thread execution
				}

				m_InputMutex.lock();
				m_InputQueue.push(std::string(szLine));
				m_InputMutex.unlock();
			}
		});
	}

	// Read the next line of input from stdin, if anything is available.
	bool Client::LocalUserInput_GetNext(std::string& result)
	{
		bool got_input = false;
		m_InputMutex.lock();
		while (!m_InputQueue.empty() && !got_input)
		{
			// get whole string until queue is empty and written to result
			result = m_InputQueue.front();
			m_InputQueue.pop();
			ValveNetworking::Utils::ltrim(result);
			ValveNetworking::Utils::rtrim(result);
			got_input = !result.empty();	// ignore blank lines
		}
		m_InputMutex.unlock();
		return got_input;
	}

	void Client::InitSteamDatagramConnectionSockets()
	{
#ifdef STEAMNETWORKINGSOCKETS_OPENSOURCE
		SteamDatagramErrMsg errMsg;
		if (!GameNetworkingSockets_Init(nullptr, errMsg))
			printf("GameNetworkingSockets_Init failed.  %s", errMsg);	// print error message
#endif
		m_LoggedTime = SteamNetworkingUtils()->GetLocalTimestamp();
	}

	void Client::ShutdownSteamDatagramConnectionSockets()
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(500));

#ifdef STEAMNETWORKINGSOCKETS_OPENSOURCE
		GameNetworkingSockets_Kill();
#else
		SteamDatagramClient_Kill();
#endif

	}

	void Client::PrintfTime(const char* msg, ...)
	{
		char text[2048];
		va_list ap;
		va_start(ap, msg);
		vsprintf(text, msg, ap);
		va_end(ap);
		char* nl = strchr(text, '\0') - 1;
		if (nl >= text && *nl == '\n')
			*nl = '\0';

		SteamNetworkingMicroseconds time = SteamNetworkingUtils()->GetLocalTimestamp() - m_LoggedTime;
		printf("%10.6f %s\n", time * 1e-6, text);
	}
}
