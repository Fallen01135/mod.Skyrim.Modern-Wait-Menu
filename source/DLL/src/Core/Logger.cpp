// Copyright (c) 2026 Fallen01135. All rights reserved.
// Licensed under the Fallen01135 Mod License (see LICENSE.md).

#include "Logger.hpp"
#include "SKSE/Impl/PCH.h"
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
		auto logsFolder = SKSE::log::log_directory();
		if (!logsFolder)
			SKSE::stl::report_and_fail("Failed to find standard logging directory");

		auto pluginName = Plugin::NAME;
		auto logFilePath = *logsFolder / std::format("{}.log", pluginName);
		auto sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(logFilePath.string(), true);
		auto level = spdlog::level::info;

		auto loggerPtr = std::make_shared<spdlog::logger>("global log", std::move(sink));

		loggerPtr->set_level(level);
		loggerPtr->flush_on(level);

		spdlog::set_default_logger(std::move(loggerPtr));

		spdlog::set_pattern("[%H:%M:%S.%e] [%l] %v");
	}
}