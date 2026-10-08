// Copyright (c) 2026 Fallen01135. All rights reserved.
// Licensed under the Fallen01135 Mod License (see LICENSE.md).

#include "../Managers/ControlManager.hpp"
#include "../Managers/MenuManager.hpp"
#include "InputEventSink.hpp"
#include "RE/B/BSTEvent.h"
#include "RE/B/ButtonEvent.h"
#include "RE/I/InputDevices.h"
#include "RE/I/InputEvent.h"
#include "RE/T/ThumbstickEvent.h"
#include "Settings.hpp"


namespace ModernWaitMenu
{
	RE::BSEventNotifyControl InputEventSink::ProcessEvent(RE::InputEvent *const *a_event,
		RE::BSTEventSource<RE::InputEvent *> *)
	{
		if (a_event && *a_event && MenuManager::IsSleepWaitMenuOpen())
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
							MenuManager::GetView(),
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
							ControlManager::sendDPadInformation(MenuManager::GetView(), "_root.SleepWaitMenu_mc.onDPadInput");
						}
					}
				}
			}
		}

		return RE::BSEventNotifyControl::kContinue;
	}
};