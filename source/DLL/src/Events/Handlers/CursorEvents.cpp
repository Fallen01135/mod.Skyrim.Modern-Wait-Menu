// Copyright (c) 2026 Fallen01135. All rights reserved.
// Licensed under the Fallen01135 Mod License (see LICENSE.md).

#include "CursorEvents.hpp"
#include "RE/G/GFxMovie.h"
#include "RE/U/UI.h"
#include "SKSE/Events.h"
#include "spdlog/spdlog.h"


namespace ModernWaitMenu
{	
	void CursorEvents::ShowMouseCursor(const SKSE::ModCallbackEvent *event)
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
}