// Copyright (c) 2026 Fallen01135. All rights reserved.
// Licensed under the Fallen01135 Mod License (see LICENSE.md).

#pragma once


#include "SKSE/Events.h"
#include <string>
#include <unordered_map>


namespace ModernWaitMenu
{
	using EventHandler = void(*)(const SKSE::ModCallbackEvent* event);

	extern const std::unordered_map<std::string, EventHandler> eventMap;
}