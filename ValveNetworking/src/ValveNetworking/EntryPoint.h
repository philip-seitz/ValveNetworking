#pragma once
#ifdef VN_PLATFORM_WINDOWS

// definition externally in apps (client/server)
extern ValveNetworking::Application* ValveNetworking::CreateApplication();

int main()
{
	printf("=============== This app is using the ValveNetworking Library ===============\n");
	auto app = ValveNetworking::CreateApplication();
	app->Run();
	delete app;			// heap allocated app instance from client or server
}

#endif