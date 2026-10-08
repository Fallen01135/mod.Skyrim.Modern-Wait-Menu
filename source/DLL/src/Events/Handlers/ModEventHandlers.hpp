// Copyright (c) 2026 Fallen01135. All rights reserved.
// Licensed under the Fallen01135 Mod License (see LICENSE.md).

#pragma once


#include "SKSE/Events.h"
#include <string>
#include <unordered_map>


using EventHandler = void(*)(const SKSE::ModCallbackEvent *event);


namespace ModernWaitMenu
{
	// --- Storage Container ---
	static std::unordered_map<int, std::string> g_savedStorage;
	// --- Storage Container end ---

	extern const std::unordered_map<std::string, EventHandler> eventMap;
}