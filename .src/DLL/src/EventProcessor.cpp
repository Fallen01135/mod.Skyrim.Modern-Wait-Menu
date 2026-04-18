#include "EventProcessor.h"
#include "Utilities.h"
#include "Settings.h"
#include "Logger.h"

namespace ModernWaitMenu
{
	using EventHandler = std::function<void(const SKSE::ModCallbackEvent*)>;

	/**
	* @brief This is a collection of all supported Mod Events that this plugin supports and uses.
	* 
	* This was made in favour of more organisation and less if/case statements.
	* More information about each entries, check the entries itself.
	* 
	* For a demonstration of how to use it check the code below of the "ModCallbackEvent".
	*/
	static std::unordered_map<std::string, EventHandler> eventMap =
	{
		/*
			MWMShowMouseCursor: If the event is called, we disable or enable the mouse cursor.
		*/
		{
			"MWMShowMouseCursor", [](const SKSE::ModCallbackEvent* a_event)
			{
				auto ui = RE::UI::GetSingleton();
				if (ui->IsMenuOpen("Cursor Menu"))
					if (auto menu = ui->GetMenu("Cursor Menu"))
						if (auto view = menu->uiMovie)
							if (view)
							{
								MWM_LOG_DEBUG("Cursor visible: {}", bool(a_event->numArg));
								view->SetVariable("_root.mc_Cursor._visible", a_event->numArg);
							}
			}
		}
	};

	RE::BSEventNotifyControl ModernWaitMenu::EventProcessor::ProcessEvent(const RE::MenuOpenCloseEvent* a_event,
		RE::BSTEventSource<RE::MenuOpenCloseEvent>* a_eventSource)
	{
		if (a_event && a_event->menuName == RE::SleepWaitMenu::MENU_NAME && a_event->opening)
		{
			// Retrieve the Menu
			auto ui = RE::UI::GetSingleton();
			auto menu = ui ? ui->GetMenu(RE::SleepWaitMenu::MENU_NAME) : nullptr;
			auto view = menu ? menu->uiMovie.get() : nullptr;

			auto settings = ModernWaitMenu::Settings::GetSingleton();

			if (view && settings)
			{
				// We get the game settings for AM and PM, so we do not need to use translation strings.
				// Fallback if not found we use AM and PM
				auto gameSettings = RE::GameSettingCollection::GetSingleton();
				std::string amStr = gameSettings ? gameSettings->GetSetting("sTimeAM")->GetString() : "AM";
				std::string pmStr = gameSettings ? gameSettings->GetSetting("sTimePM")->GetString() : "PM";
				if (!gameSettings)
					MWM_LOG_WARN("Game Settings could not be loaded, using pre defined AM and PM instead.");

				// This sets some variables inside of the ActionScript 2 code of the Menu
				const int size = 4;
				RE::GFxValue args[size];
				args[0].SetString(amStr);
				args[1].SetString(pmStr);
				args[2].SetBoolean(settings->bUseLeadingZero);
				args[3].SetBoolean(settings->bUse24Clock);

				for (int i = 0; i < size; i++)
					view->SetVariable(fmt::format("_root.SleepWaitMenu_mc.{}", as2VarNames[i]).c_str(), args[i]);

				// Run other functions
				ModernWaitMenu::TimeManager::UpdateMenuTime(view, true);
				ModernWaitMenu::WeatherManager::updateCurrentWeather(view, true);

				MWM_LOG_DEBUG("Wait menu opened.");
			}
			else if (!view)
				MWM_LOG_CRITICAL("SleepWaitMenu could not be found and opened!");
			else if (!settings)
				MWM_LOG_CRITICAL("Settings could not be loaded!");
		}

		return RE::BSEventNotifyControl::kContinue;
	};

	RE::BSEventNotifyControl EventProcessor::ProcessEvent(const SKSE::ModCallbackEvent* a_event,
		RE::BSTEventSource<SKSE::ModCallbackEvent>* a_eventSource)
	{
		if (a_event)
		{
			// This will check if the string we just recieced is found as a key in our eventMap
			auto it = eventMap.find(a_event->eventName.c_str());
			if (it != eventMap.end())
			{
				MWM_LOG_DEBUG("Mod Event was called: {}", a_event->eventName.c_str());
				it->second(a_event); // If we found the key, we run the code that is attached to it.
			}
			else
				MWM_LOG_DEBUG("Mod Event not registered: {}", a_event->eventName.c_str());
		}

		return RE::BSEventNotifyControl::kContinue;
	}
};