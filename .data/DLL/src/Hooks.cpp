#include "Hooks.hpp"
#include "Managers.hpp"
#include "RE/I/IMenu.h"
#include "RE/Offsets_VTABLE.h"
#include "RE/S/SleepWaitMenu.h"
#include "RE/U/UIMessage.h"
#include "REL/Relocation.h"
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
		REL::Relocation<std::uintptr_t> vTable(RE::VTABLE_SleepWaitMenu[0]);
		_ProcessMessage = vTable.write_vfunc(0x4, &ProcessMessage_Hook);
	}
}