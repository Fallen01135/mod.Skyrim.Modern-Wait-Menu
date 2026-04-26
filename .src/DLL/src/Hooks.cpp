#include "logger.h"
#include "Hooks.h"
#include "Utilities.h"


namespace ModernWaitMenu
{
	RE::UI_MESSAGE_RESULTS SleepWaitMenuHook::ProcessMessage_Hook(RE::SleepWaitMenu* a_this, RE::UIMessage& a_message)
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
				MWM_LOG_DEBUG("Menu not found, skipping");
		}

		return _ProcessMessage(a_this, a_message);
	}

	void SleepWaitMenuHook::Install()
	{
		REL::Relocation<std::uintptr_t> vTable(RE::VTABLE_SleepWaitMenu[0]);
		_ProcessMessage = vTable.write_vfunc(0x4, &ProcessMessage_Hook);
	}
}