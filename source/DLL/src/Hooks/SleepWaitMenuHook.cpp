// Copyright (c) 2026 Fallen01135. All rights reserved.
// Licensed under the Fallen01135 Mod License (see LICENSE.md).

#include "../Managers/TimeManager.hpp"
#include "../Managers/WeatherManager.hpp"
#include "../Platform/Offset.hpp"
#include "RE/I/IMenu.h"
#include "RE/S/SleepWaitMenu.h"
#include "RE/U/UIMessage.h"
#include "REL/Relocation.h"
#include "SleepWaitMenuHook.hpp"
#include "spdlog/spdlog.h"
#include <cstdarg>


namespace ModernWaitMenu
{
	RE::UI_MESSAGE_RESULTS SleepWaitMenuHook::ProcessMessage_Hook(RE::SleepWaitMenu *a_this, RE::UIMessage &a_message)
	{
		if (a_message.type == RE::UI_MESSAGE_TYPE::kUpdate && a_this && a_this->uiMovie)
		{
			auto view = a_this->uiMovie.get();
			if (view)
			{
				TimeManager::UpdateMenuTime(view, false);
				WeatherManager::updateCurrentWeather(view, false);
			}
			else
				spdlog::debug("Menu not found, skipping");
		}

		return _ProcessMessage(a_this, a_message);
	}

	void SleepWaitMenuHook::Install()
	{
		REL::Relocation<std::uintptr_t> vTable{ Offset::SleepWaitMenu::Vtbl.address() };
		_ProcessMessage = vTable.write_vfunc(0x4, &ProcessMessage_Hook);
	}
}