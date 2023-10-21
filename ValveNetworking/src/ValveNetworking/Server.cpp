#define _CRT_SECURE_NO_WARNINGS
#include "Server.h"
#include "Client.h"
#include "NetworkingUtils.h"
#include <thread>
#include <mutex>
#include <queue>
#include <stdarg.h>

#ifdef _WIN32
#include <windows.h>		// For StopProcess
#else
#include <unistd.h>
#include <signal.h>
#endif

namespace ValveNetworking
{
	Server* s_CallbackInstance = nullptr;
	void Server::Run(uint16 port)
	{
		m_Running = true;
		InitSteamDatagramConnectionSockets();			// init API
		LocalUserInput_Init();							// setup user input thread (~non blocking)
		m_Interface = SteamNetworkingSockets();
		m_Port = port;

		// start Listener
		SteamNetworkingIPAddr serverLocalAddr;
		serverLocalAddr.Clear();						// IP set to zero
		serverLocalAddr.m_port = m_Port;				// server listening port
		SteamNetworkingConfigValue_t options;			// options stored in struct (e.g. pointer storing status changed callback)
		options.SetPtr(k_ESteamNetworkingConfig_Callback_ConnectionStatusChanged, (void*)NetConnectionStatusChangedCallback);
		m_ListenSocket = m_Interface->CreateListenSocketIP(serverLocalAddr, 1, &options);	// doesn't use IP; client rather connects to server IP 
		if (m_ListenSocket == k_HSteamListenSocket_Invalid)
			PrintfTime("Failed to listen on port %d", m_Port);

		m_PollGroup = m_Interface->CreatePollGroup();
		if (m_PollGroup == k_HSteamNetPollGroup_Invalid)
			PrintfTime("Failed to listen on port %d", m_Port);

		PrintfTime("Server listening on port %d\n", m_Port);

		while (m_Running)
		{
			HandleIncomingMessages();					// poll messages coming from connection (+print)
			HandleConnectionStateChanges();				// print notis on connections/dcs etc.
			HandleLocalUserInput();						// poll user messages and send to connection
			std::this_thread::sleep_for(std::chrono::milliseconds(10));		// reduce polling frequency to lower load
		}

		ShutdownSteamDatagramConnectionSockets();
		StopProcess(0);
	}

	void Server::StopProcess(int rc)
	{
#ifdef _WIN32
			ExitProcess(rc);
#endif
	}
		
	void Server::LocalUserInput_Init()
	{
		m_InputThread = new std::thread([this]()
			{
				// thread code defined via lambda function
				while (m_Running)
				{
					char szLine[4000];
					if (!fgets(szLine, sizeof(szLine), stdin))		// write user input (console) to szLine 
					{
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
	bool Server::LocalUserInput_GetNext(std::string& result)
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

	void Server::PrintfTime(const char* msg, ...)
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

	void Server::InitSteamDatagramConnectionSockets()
	{
#ifdef STEAMNETWORKINGSOCKETS_OPENSOURCE
		SteamDatagramErrMsg errMsg;
		if (!GameNetworkingSockets_Init(nullptr, errMsg))
			printf("GameNetworkingSockets_Init failed.  %s", errMsg);	// print error message
#endif
		m_LoggedTime = SteamNetworkingUtils()->GetLocalTimestamp();
	}

	void Server::ShutdownSteamDatagramConnectionSockets()
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(500));

#ifdef STEAMNETWORKINGSOCKETS_OPENSOURCE
		GameNetworkingSockets_Kill();
#else
		SteamDatagramClient_Kill();
#endif

	}

	void Server::SendStringToClient(HSteamNetConnection conn, const char* str)
	{
		m_Interface->SendMessageToConnection(conn, str, (uint32)strlen(str), k_nSteamNetworkingSend_Reliable, nullptr);
	}

	void Server::SendStringToAllClients(const char* str, HSteamNetConnection except)
	{
		for (std::map< HSteamNetConnection, ClientData>::iterator c = m_Clients.begin(); c != m_Clients.end(); c++)
		{
			if (c->first != except)
				SendStringToClient(c->first, str);
		}
	}

	void Server::SendFloatToClient(HSteamNetConnection conn, uint8 nmbr)
	{
		uint32 size = 1;
		void* data = new uint8[size];
		*(uint8*)data = nmbr;
		m_Interface->SendMessageToConnection(conn, &data, uint32(size), k_nSteamNetworkingSend_Reliable, nullptr);
		delete[] data;
	}

	void Server::HandleIncomingMessages()
	{
		char temp[1024];

		while (m_Running)
		{
			ISteamNetworkingMessage* pIncomingMsg = nullptr;
			int numMsgs = m_Interface->ReceiveMessagesOnPollGroup(m_PollGroup, &pIncomingMsg, 1);
			if (numMsgs == 0)
				break;
			if (numMsgs < 0)
				PrintfTime("Error checking for messages");
			auto itClient = m_Clients.find(pIncomingMsg->m_conn);	// iterator for what client from or to (connection)

			// '\0'-terminate it to make it easier to parse {-> std::string}
			std::string sCmd;
			sCmd.assign((const char*)pIncomingMsg->m_pData, pIncomingMsg->m_cbSize);
			const char* cmd = sCmd.c_str();		// write message to sCmd (string)

			// We don't need this anymore.
			pIncomingMsg->Release();

			if (strncmp(cmd, "/nick", 5) == 0)		// max 5 characters
			{
				const char* nick = cmd + 5;			// move pointer to place after nickname
				while (isspace(*nick))				// check for space/tab/newline
					++nick;

				// Let everybody else know they changed their name (evry1 except name changer)
				sprintf(temp, "%s renamed to %s", itClient->second.nickname.c_str(), nick);
				// replacing default value it should send it to all clients exept the one who renamed
				SendStringToAllClients(temp, itClient->first);

				// Respond to client (namechanger)
				sprintf(temp, "You renamed to %s", nick);
				SendStringToClient(itClient->first, temp);
				
				// Actually change their name
				SetClientNickname(itClient->first, nick);
				continue;
			}

			if (strcmp(cmd, "MOVE_UP") == 0)
			{
				SendStringToClient(itClient->first, "MOVE_UP");
			}

			else if (strcmp(cmd, "MOVE_DOWN") == 0)
			{
				SendStringToClient(itClient->first, "MOVE_DOWN");
			}

			// Assume it's just a ordinary chat message, dispatch to everybody else
			sprintf(temp, "%s: %s", itClient->second.nickname.c_str(), cmd);
			SendStringToAllClients(temp, itClient->first);
		}
	}

	void Server::HandleLocalUserInput()
	{
		std::string cmd;
		while (m_Running && LocalUserInput_GetNext(cmd))
		{
			if (strcmp(cmd.c_str(), "/quit") == 0)
			{
				m_Running = false;
				PrintfTime("Shutting down server");
				break;
			}
			// That's the only command we support
			PrintfTime("The server only knows one command: '/quit'");
		}

	}

	void Server::SetClientNickname(HSteamNetConnection hConn, const char* nickname)
	{
		// Remember their nick
		m_Clients[hConn].nickname = nickname;

		// Set the connection name, too, which is useful for debugging
		m_Interface->SetConnectionName(hConn, nickname);
	}

	// API intern callback stores connection states in steamNet...Callback_t struct and queues them
	// data (struct) of different callbacks can then be used to send messages
	void Server::OnNetConnectionStatusChanged(SteamNetConnectionStatusChangedCallback_t* pInfo)
	{
		char temp[1024];

		// What's the state of the connection?
		switch (pInfo->m_info.m_eState)
		{
		case k_ESteamNetworkingConnectionState_None:
			// NOTE: We will get callbacks here when we destroy connections.  You can ignore these.
			break;

		case k_ESteamNetworkingConnectionState_ClosedByPeer:
		case k_ESteamNetworkingConnectionState_ProblemDetectedLocally:
		{
			// Ignore if they were not previously connected.  (If they disconnected
			// before we accepted the connection.)
			if (pInfo->m_eOldState == k_ESteamNetworkingConnectionState_Connected)
			{

				// Locate the client.  Note that it should have been found, because this
				// is the only codepath where we remove clients (except on shutdown),
				// and connection change callbacks are dispatched in queue order.
				auto itClient = m_Clients.find(pInfo->m_hConn);

				// Select appropriate log messages
				const char* pszDebugLogAction;
				if (pInfo->m_info.m_eState == k_ESteamNetworkingConnectionState_ProblemDetectedLocally)
				{
					pszDebugLogAction = "problem detected locally";
					sprintf(temp, "Alas, %s hath fallen into shadow.  (%s)", itClient->second.nickname.c_str(), pInfo->m_info.m_szEndDebug);
				}
				else
				{
					// Note that here we could check the reason code to see if
					// it was a "usual" connection or an "unusual" one.
					pszDebugLogAction = "closed by peer";
					sprintf(temp, "%s hath departed", itClient->second.nickname.c_str());
				}

				// Spew something to our own log.  Note that because we put their nick
				// as the connection description, it will show up, along with their
				// transport-specific data (e.g. their IP address)
				PrintfTime("Connection %s %s, reason %d: %s\n",
					pInfo->m_info.m_szConnectionDescription,
					pszDebugLogAction,
					pInfo->m_info.m_eEndReason,
					pInfo->m_info.m_szEndDebug
				);

				m_Clients.erase(itClient);

				// Send a message so everybody else knows what happened
				SendStringToAllClients(temp);
			}
			else
			{
			}

			// Clean up the connection.  This is important!
			// The connection is "closed" in the network sense, but
			// it has not been destroyed.  We must close it on our end, too
			// to finish up.  The reason information do not matter in this case,
			// and we cannot linger because it's already closed on the other end,
			// so we just pass 0's.
			m_Interface->CloseConnection(pInfo->m_hConn, 0, nullptr, false);
			break;
		}

		case k_ESteamNetworkingConnectionState_Connecting:
		{
			// This must be a new connection
			PrintfTime("Connection request from %s", pInfo->m_info.m_szConnectionDescription);

			// A client is attempting to connect
			// Try to accept the connection.
			if (m_Interface->AcceptConnection(pInfo->m_hConn) != k_EResultOK)
			{
				// This could fail.  If the remote host tried to connect, but then
				// disconnected, the connection may already be half closed.  Just
				// destroy whatever we have on our side.
				m_Interface->CloseConnection(pInfo->m_hConn, 0, nullptr, false);
				PrintfTime("Can't accept connection.  (It was already closed?)");
				break;
			}

			// Assign the poll group
			if (!m_Interface->SetConnectionPollGroup(pInfo->m_hConn, m_PollGroup))
			{
				m_Interface->CloseConnection(pInfo->m_hConn, 0, nullptr, false);
				PrintfTime("Failed to set poll group?");
				break;
			}

			// Generate a random nick.  A random temporary nick
			// is really dumb and not how you would write a real chat server.
			// You would want them to have some sort of signon message,
			// and you would keep their client in a state of limbo (connected,
			// but not logged on) until them.  I'm trying to keep this example
			// code really simple.
			char nick[64];
			sprintf(nick, "BraveWarrior%d", 10000 + (rand() % 100000));

			// Send them a welcome message
			sprintf(temp, "Welcome, stranger.  Thou art known to us for now as '%s'; upon thine command '/nick' we shall know thee otherwise.", nick);
			SendStringToClient(pInfo->m_hConn, temp);

			// Also send them a list of everybody who is already connected
			if (m_Clients.empty())
			{
				SendStringToClient(pInfo->m_hConn, "Thou art utterly alone.");
			}
			else
			{
				sprintf(temp, "%d companions greet you:", (int)m_Clients.size());
				for (auto& c : m_Clients)
					SendStringToClient(pInfo->m_hConn, c.second.nickname.c_str());
			}

			// Let everybody else know who they are for now
			sprintf(temp, "Hark!  A stranger hath joined this merry host.  For now we shall call them '%s'", nick);
			SendStringToAllClients(temp, pInfo->m_hConn);

			// Add them to the client list, using std::map wacky syntax
			m_Clients[pInfo->m_hConn];
			SetClientNickname(pInfo->m_hConn, nick);
			break;
		}

		case k_ESteamNetworkingConnectionState_Connected:
			// We will get a callback immediately after accepting the connection.
			// Since we are the server, we can ignore this, it's not news to us.
			break;

		default:
			// Silences -Wswitch
			break;
		}
	}

	void Server::NetConnectionStatusChangedCallback(SteamNetConnectionStatusChangedCallback_t* pInfo)
	{
		s_CallbackInstance->OnNetConnectionStatusChanged(pInfo);
		// static (callback)-function needs a reference to an actual object
	}

	void Server::HandleConnectionStateChanges()
	{
		s_CallbackInstance = this;
		m_Interface->RunCallbacks();	// run queued callbacks
	}

}