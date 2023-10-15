#pragma once

#ifdef VN_PLATFORM_WINDOWS
	#ifdef VN_BUILD_DLL
		#define VN_API __declspec(dllexport)
	#else
		#define VN_API __declspec(dllimport)
	#endif 
#else 
	#error This application only supports Windows!
#endif 