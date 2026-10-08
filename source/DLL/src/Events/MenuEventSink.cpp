// Copyright (c) 2026 Fallen01135. All rights reserved.
// Licensed under the Fallen01135 Mod License (see LICENSE.md).

#include "../Managers/MenuManager.hpp"
#include "../Managers/SettingsManager.hpp"
#include "../Managers/TimeManager.hpp"
#include "../Managers/WeatherManager.hpp"
#include "MenuEventSink.hpp"
#include "RE/B/BSTEvent.h"
#include "RE/G/GFxValue.h"
#include "RE/G/GameSettingCollection.h"
#include "RE/M/MenuOpenCloseEvent.h"
#include "RE/S/SleepWaitMenu.h"
#include "RE/U/UI.h"
#include "Settings.hpp"
#include "spdlog/spdlog.h"
#include <string>


namespace ModernWaitMenu
{
	RE::BSEventNotifyControl MenuEventSink::ProcessEvent(const RE::MenuOpenCloseEvent *a_event,
		RE::BSTEventSource<RE::MenuOpenCloseEvent> *)
	{
		if (a_event && a_event->opening && a_event->menuName == RE::SleepWaitMenu::MENU_NAME)
		{
			// Retrieve the Menu
			auto ui = RE::UI::GetSingleton();
			auto menu = ui ? ui->GetMenu(RE::SleepWaitMenu::MENU_NAME) : nullptr;
			auto view = menu ? menu->uiMovie.get() : nullptr;

			if (view)
			{
				MenuManager::SetView(view);
				ModernWaitMenu::SettingsManager::Load();

				// We get the game settings for AM and PM, so we do not need to use translation strings.
				// Fallback if not found we use AM and PM
				auto gameSettings = RE::GameSettingCollection::GetSingleton();
				std::string amStr = gameSettings ? gameSettings->GetSetting("sTimeAM")->GetString() : "AM";
				std::string pmStr = gameSettings ? gameSettings->GetSetting("sTimePM")->GetString() : "PM";
				if (!gameSettings)
					spdlog::warn("Game Settings could not be loaded, using pre defined AM and PM instead.");

				// This sets some variables inside of the ActionScript 2 code of the Menu
				const size_t size = std::size(as2VarNames);
				int index = 0;
				RE::GFxValue args[size];
				args[index++].SetString(amStr);
				args[index++].SetString(pmStr);
				args[index++].SetBoolean(Settings::bUseLeadingZero);
				args[index++].SetBoolean(Settings::bUse24Clock);
				args[index++].SetBoolean(Settings::bUseCustomCursor);

				if (index == size)
				{
					int argIndex = 0;
					for (const auto &miep : as2VarNames)
						view->SetVariable(std::format("_root.SleepWaitMenu_mc.{}", miep).c_str(), args[argIndex++]);
				}
				else
					spdlog::critical("Argument count not correct! Size: {}; Index: {}", size, index);

				// For VR compatibility
				RE::GFxValue arg;
				arg.SetBoolean(SettingsManager::isVR());
				view->Invoke("_root.SleepWaitMenu_mc.setVR", nullptr, &arg, 1);

				// Run other functions
				TimeManager::UpdateMenuTime(view, true);
				WeatherManager::updateCurrentWeather(view, true);

				spdlog::debug("Wait menu opened.");
			}
			else
				spdlog::critical("SleepWaitMenu could not be found and opened!");
		}
		else if (a_event && !a_event->opening && a_event->menuName == RE::SleepWaitMenu::MENU_NAME)
			MenuManager::SetView(nullptr);

		return RE::BSEventNotifyControl::kContinue;
	};
};