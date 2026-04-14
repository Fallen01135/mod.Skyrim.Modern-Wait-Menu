#pragma once
#include <spdlog/spdlog.h>

namespace ModernWaitMenu
{
	class Logger
	{
	private:
	public:
		/**
		* @brief This initializes the custom logging function and creates a custom log file
		*/
		static void Init();
	};
}

#define MWM_LOG(level, ...) \
	if (spdlog::get_level() <= level) \
		spdlog::log(spdlog::source_loc{__FILE__, __LINE__, __func__}, level, __VA_ARGS__)

#define MWM_LOG_TRACE(...) MWM_LOG(spdlog::level::trace, __VA_ARGS__)
#define MWM_LOG_DEBUG(...) MWM_LOG(spdlog::level::debug, __VA_ARGS__)

#define MWM_LOG_INFO(...) MWM_LOG(spdlog::level::info, __VA_ARGS__)
#define MWM_LOG_WARN(...) MWM_LOG(spdlog::level::warn, __VA_ARGS__)

#define MWM_LOG_ERROR(...) MWM_LOG(spdlog::level::err, __VA_ARGS__)
#define MWM_LOG_CRITICAL(...) MWM_LOG(spdlog::level::critical, __VA_ARGS__)