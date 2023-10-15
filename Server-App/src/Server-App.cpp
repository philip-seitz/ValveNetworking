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

// We do this because I won't want to figure out how to cleanly shut
// down the thread that is reading from stdin.
static void NukeProcess(int rc)
{
#ifdef _WIN32
	ExitProcess(rc);
#endif
}

const uint16 DEFAULT_SERVER_PORT = 27020;

int main()
{
	SteamNetworkingIPAddr addrServer;	// Server IP address
	addrServer.Clear();					// server IP and port set to zero

	ValveNetworking::Server server;
	server.Run((uint16)DEFAULT_SERVER_PORT);

	NukeProcess(0);
}