// Copyright (c) 2026 Fallen01135. All rights reserved.
// Licensed under the Fallen01135 Mod License (see LICENSE.md).

#include "Settings.hpp"
#include "SettingsManager.hpp"
#include "SimpleSettings.hpp"

#ifdef SKYRIMVR
#include "REL/Module.h"
#endif


namespace ModernWaitMenu
{
	void SettingsManager::Load()
	{
		// Retrieve the Plugin name and get the ini file.
		auto pluginName = Plugin::NAME;
		std::string iniPath = std::format("Data/SKSE/Plugins/{}.ini", pluginName);

		SettingsLib::SetCommentStyle(SettingsLib::CommentStyle::Hash);
		SettingsLib::Load<Settings>(iniPath, SettingsLib::LoadMode::ReadAndRepair);

		applySettings();
	}

	void SettingsManager::applySettings()
	{
#ifndef NDEBUG
		spdlog::set_level(spdlog::level::trace);
		spdlog::flush_on(spdlog::level::trace);
#else
		const auto level = Settings::bExtraLogging ? spdlog::level::trace : spdlog::level::info;
		spdlog::set_level(level);
		spdlog::flush_on(level);
#endif

#ifdef SKYRIMVR
		// Check if we are using VR or SE below 1.6
		_isVR = REL::Module::get().GetRuntime() == REL::Module::Runtime::VR;
#endif

		SettingsLib::Dump<Settings>
			(
				[](const auto& setting)
				{
					spdlog::debug(std::format("Loaded Settings {} with value {}", setting.info.key, setting));
				}
			);
	}
}