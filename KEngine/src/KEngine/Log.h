#pragma once

#include<memory>

#include "Core.h"
#include"spdlog\spdlog.h"

namespace KEngine
{	
	class KE_API Log
	{
	private:
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_ClientLogger;

	public:

		static void Init();

		inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
		inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }
	};

}

//Core log macros
#define KE_CORE_ERROR(...) ::KEngine::Log::GetCoreLogger()->error(__VA_ARGS__)
#define KE_CORE_WARN(...)  ::KEngine::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define KE_CORE_INFO(...)  ::KEngine::Log::GetCoreLogger()->info(__VA_ARGS__)
#define KE_CORE_TRACE(...) ::KEngine::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define KE_CORE_FATAL(...) ::KEngine::Log::GetCoreLogger()->fatal(__VA_ARGS__)

//Client log macros
#define KE_CLIENT_ERROR(...) ::KEngine::Log::GetClientLogger()->error(__VA_ARGS__)
#define KE_CLIENT_WARN(...)  ::KEngine::Log::GetClientLogger()->warn(__VA_ARGS__)
#define KE_CLIENT_INFO(...)  ::KEngine::Log::GetClientLogger()->info(__VA_ARGS__)
#define KE_CLIENT_TRACE(...) ::KEngine::Log::GetClientLogger()->trace(__VA_ARGS__)
#define KE_CLIENT_FATAL(...) ::KEngine::Log::GetClientLogger()->fatal(__VA_ARGS__)