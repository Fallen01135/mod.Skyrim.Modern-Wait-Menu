#include "Callbacks.hpp"
#include "RE/G/GFxMovie.h"
#include "RE/U/UI.h"
#include "SKSE/Events.h"
#include "spdlog/spdlog.h"
#include <string>
#include <unordered_map>


namespace ModernWaitMenu
{	
	void MWMShowMouseCursor(const SKSE::ModCallbackEvent *event)
	{
		if (!event)
			return;

		auto ui = RE::UI::GetSingleton();
		if (!ui || !ui->IsMenuOpen("Cursor Menu"))
			return;

		auto menu = ui->GetMenu("Cursor Menu");
		if (!menu || !menu->uiMovie)
			return;

		auto view = menu->uiMovie.get();
		spdlog::debug("Cursor visible: {}", bool(event->numArg));
		view->SetVariable("_root.mc_Cursor._visible", event->numArg);
	}


	const std::unordered_map<std::string, EventHandler> eventMap =
	{
		{"MWMShowMouseCursor", MWMShowMouseCursor}
	};
}