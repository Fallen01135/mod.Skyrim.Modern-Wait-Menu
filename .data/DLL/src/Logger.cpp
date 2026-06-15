#include "Logger.hpp"
#include "SKSE/Impl/PCH.h"
#include "SKSE/Interfaces.h"
#include "SKSE/Logger.h"
#include "spdlog/common.h"
#include "spdlog/logger.h"
#include "spdlog/spdlog.h"
#include <format>
#include <memory>
#include <spdlog/sinks/basic_file_sink.h>
#include <utility>


namespace ModernWaitMenu
{
	void Logger::Init()
	{
		// No idea why, but this points to the "My Games/Skyrim.INI" folder.
		auto logsFolder = SKSE::log::log_directory();
		if (!logsFolder)
			SKSE::stl::report_and_fail("!logsFolder");

		auto pluginName = SKSE::PluginDeclaration::GetSingleton()->GetName();
		auto logFilePath = *logsFolder / std::format("{}.log", pluginName);
		auto fileLoggerPtr = std::make_shared<spdlog::sinks::basic_file_sink_mt>(logFilePath.string(), true);
		auto loggerPtr = std::make_shared<spdlog::logger>("log", std::move(fileLoggerPtr));

		spdlog::set_default_logger(std::move(loggerPtr));

		spdlog::set_pattern("[%H:%M:%S.%e] [%l] %v");

		spdlog::set_level(spdlog::level::info);
		spdlog::flush_on(spdlog::level::info);
	}
}