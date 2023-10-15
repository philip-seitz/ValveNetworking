#pragma once
#include "Core.h"

namespace ValveNetworking
{
	class VN_API Application
	{
	public:
		Application();
		virtual ~Application();			// without virtual the derived Constructor will not be called

		virtual void Run();

	private:




	};

	// Implementation template for Client
	VN_API Application* CreateApplication();
}

