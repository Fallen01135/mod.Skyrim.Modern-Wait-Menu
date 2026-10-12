// Copyright (c) 2026 Fallen01135. All rights reserved.
// Licensed under the Fallen01135 Mod License (see LICENSE.md).

#include "CursorEvents.hpp"
#include "ModEventHandlers.hpp"
#include <string>
#include <unordered_map>


namespace ModernWaitMenu
{
	constexpr std::string_view tag = "FTA_MWM_";

	const std::unordered_map<std::string, EventHandler> eventMap =
	{
		{std::string(tag) + "ShowMouseCursor", CursorEvents::ShowMouseCursor}
	};
}