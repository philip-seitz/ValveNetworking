#define _CRT_SECURE_NO_WARNINGS
#include "Client.h"
#include <thread>
#include <mutex>
#include <queue>
#include <string>
#include <stdarg.h>
#include <windows.h>

#include "NetworkingUtils.h"

namespace ValveNetworking
{
	Client* s_CallbackInstance = nullptr;

	void Client::Run(const SteamNetworkingIPAddr& serverAddr)
	{
		m_Running = false;
		InitSteamDatagramConnectionSockets();		// init API
		LocalUserInput_Init();						// setup user input thread (~non blocking)

		m_Interface = SteamNetworkingSockets();

		// Start connecting
		char address[SteamNetworkingIPAddr::k_cchMaxString];
		serverAddr.ToString(address, sizeof(address), true);						// write IP to szAddr
		PrintfTime("Connecting to chat server at %s", address);
		SteamNetworkingConfigValue_t options;
		options.SetPtr(k_ESteamNetworkingConfig_Callback_ConnectionStatusChanged, (void*)NetConnectionStatusChangedCallback);
		m_Connection = m_Interface->ConnectByIPAddress(serverAddr, 1, &options);	// try IP to connect
		if (m_Connection == k_HSteamNetConnection_Invalid)
			PrintfTime("Failed to create connection");

		while (!m_Running)
		{
			HandleIncomingMessages();										// poll messages coming over connection (+print)
			HandleConnectionStateChanges();									// print messages depending on connection/dcs etc.
			HandleLocalUserInput();											// poll user messages and send them over connection
			std::this_thread::sleep_for(std::chrono::milliseconds(10));		// reduce polling frequency to lower load
		}
		ShutdownSteamDatagramConnectionSockets();
	}

	void Client::StopProcess(int rc)
	{
#ifdef _WIN32
		ExitProcess(rc);
#endif
	}

	void Client::HandleIncomingMessages()
	{
		while (!m_Running)
		{
			ISteamNetworkingMessage* pIncomingMsg = nullptr;
			int numMsgs = m_Interface->ReceiveMessagesOnConnection(m_Connection, &pIncomingMsg, 1);
			if (numMsgs == 0)
				break;
			if (numMsgs < 0)
				PrintfTime("Error checking for messages");

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
		while (!m_Running && LocalUserInput_GetNext(cmd))		// read in new message (cmd) if any return true
		{
			// Check for known commands
			if (strcmp(cmd.c_str(), "/quit") == 0)
			{
				m_Running = true;
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
			m_Running = true;

			// Print an appropriate message
			if (pInfo->m_eOldState == k_ESteamNetworkingConnectionState_Connecting)	// m_eOldState info of last state
			{
				// Note: we could distinguish between a timeout, a rejected connection,
				// or some other transport problem. (connection failed)
				PrintfTime("We sought the remote host, yet our efforts were met with defeat.  (%s)", pInfo->m_info.m_szEndDebug);
			}
			else if (pInfo->m_info.m_eState == k_ESteamNetworkingConnectionState_ProblemDetectedLocally)
			{
				PrintfTime("Alas, troubles beset us; we have lost contact with the host.  (%s)", pInfo->m_info.m_szEndDebug);
			}
			else
			{
				// NOTE: We could check the reason code for a normal disconnection
				PrintfTime("The host hath bidden us farewell.  (%s)", pInfo->m_info.m_szEndDebug);
			}

			// Clean up the connection.  This is important!
			// The connection is "closed" in the network sense, but
			// it has not been destroyed.  We must close it on our end, too
			// to finish up.  The reason information do not matter in this case,
			// and we cannot linger because it's already closed on the other end,
			// so we just pass 0's.
			m_Interface->CloseConnection(pInfo->m_hConn, 0, nullptr, false);
			m_Connection = k_HSteamNetConnection_Invalid;
			break;
		}

		case k_ESteamNetworkingConnectionState_Connecting:
			// We will get this callback when we start connecting.
			// We can ignore this.
			break;

		case k_ESteamNetworkingConnectionState_Connected:
			PrintfTime("Connected to server OK");
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
				while (!m_Running)
				{
					char szLine[4000];
					if (!fgets(szLine, sizeof(szLine), stdin))		// write user input (console) to szLine 
					{
						// Well, you would hope that you could close the handle
						// from the other thread to trigger this.  Nope.
						if (m_Running)
							return;
						m_Running = true;
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
