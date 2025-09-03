#pragma once

#ifdef KENGINE_PLATFORM_WINDOWS 
	#ifdef KENGINE_BUILD_DLL
		#define KE_API __declspec(dllexport)
	#else
		#define KE_API __declspec(dllimport)
	#endif
#else 
	#error KEngine only supports Windows!
#endif

#define BIT(x) (1 << x)