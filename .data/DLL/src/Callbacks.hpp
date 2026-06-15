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