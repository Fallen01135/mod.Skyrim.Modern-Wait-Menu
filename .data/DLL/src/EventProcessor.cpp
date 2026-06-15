#include "Callbacks.hpp"
#include "EventProcessor.hpp"
#include "Managers.hpp"
#include "RE/B/BSTEvent.h"
#include "RE/B/ButtonEvent.h"
#include "RE/G/GameSettingCollection.h"
#include "RE/G/GFxValue.h"
#include "RE/I/InputDevices.h"
#include "RE/I/InputEvent.h"
#include "RE/M/MenuOpenCloseEvent.h"
#include "RE/S/SleepWaitMenu.h"
#include "RE/T/ThumbstickEvent.h"
#include "RE/U/UI.h"
#include "Settings.hpp"
#include "SKSE/Events.h"
#include "spdlog/spdlog.h"
#include <format>
#include <iterator>
#include <list>
#include <string>


namespace ModernWaitMenu
{
	RE::BSEventNotifyControl EventProcessor::ProcessEvent(const RE::MenuOpenCloseEvent *a_event,
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
				setView(view);
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
				view->Invoke("_root.SleepWaitMenu_mc.setVR", nullptr, &arg, args->GetArraySize());

				// Run other functions
				TimeManager::UpdateMenuTime(view, true);
				WeatherManager::updateCurrentWeather(view, true);

				spdlog::debug("Wait menu opened.");
			}
			else
				spdlog::critical("SleepWaitMenu could not be found and opened!");
		}
		else if (a_event && !a_event->opening && a_event->menuName == RE::SleepWaitMenu::MENU_NAME)
			setView(nullptr);

		return RE::BSEventNotifyControl::kContinue;
	};

	RE::BSEventNotifyControl EventProcessor::ProcessEvent(const SKSE::ModCallbackEvent *a_event,
		RE::BSTEventSource<SKSE::ModCallbackEvent> *)
	{
		if (a_event)
		{
			// This will check if the string we just recieced is found as a key in our eventMap
			auto it = eventMap.find(a_event->eventName.c_str());
			if (it != eventMap.end())
			{
				spdlog::debug("Mod Event was called: {}", a_event->eventName.c_str());
				it->second(a_event); // If we found the key, we run the code that is attached to it.
			}
			else
				spdlog::debug("Mod Event not registered: {}", a_event->eventName.c_str());
		}

		return RE::BSEventNotifyControl::kContinue;
	}

	RE::BSEventNotifyControl EventProcessor::ProcessEvent(RE::InputEvent *const *a_event,
		RE::BSTEventSource<RE::InputEvent *> *)
	{
		if (a_event && *a_event && isSleepWaitMenuOpen())
		{
			for (auto event = *a_event; event; event = event->next)
			{
				auto type = event->GetEventType();
				if (Settings::bActivateLeftStick && type == RE::INPUT_EVENT_TYPE::kThumbstick)
				{
					auto thumbstick = static_cast<RE::ThumbstickEvent *>(event);
					if (thumbstick->IsLeft())
						ControlManager::sendStickInformation
						(
							getView(),
							"_root.SleepWaitMenu_mc.onStickLeft",
							ControlManager::StickType::left,
							thumbstick->xValue,
							thumbstick->yValue
						);
				}
				else if (type == RE::INPUT_EVENT_TYPE::kButton)
				{
					auto button = static_cast<RE::ButtonEvent *>(event);
					if (button->GetDevice() == RE::INPUT_DEVICE::kGamepad)
					{
						ControlManager::DPadType id = static_cast<ControlManager::DPadType>(button->idCode);
						if
							(
								id == ControlManager::DPadType::left ||
								id == ControlManager::DPadType::right
							)
						{
							ControlManager::updateDPad(button->idCode, button->IsPressed());
							ControlManager::sendDPadInformation(getView(), "_root.SleepWaitMenu_mc.onDPadInput");
						}
					}
				}
			}
		}

		return RE::BSEventNotifyControl::kContinue;
	}
};