// Copyright (c) 2026 Fallen01135. All rights reserved.
// Licensed under the Fallen01135 Mod License (see LICENSE.md).

#include "Handlers/ModEventHandlers.hpp"
#include "ModEventSink.hpp"
#include "RE/B/BSTEvent.h"
#include "SKSE/Events.h"
#include "spdlog/spdlog.h"
#include <list>


namespace ModernWaitMenu
{
	RE::BSEventNotifyControl ModEventSink::ProcessEvent(const SKSE::ModCallbackEvent *a_event,
		RE::BSTEventSource<SKSE::ModCallbackEvent> *)
	{
		if (a_event)
		{
			// This will check if the string we just recieced is found as a key in our eventMap
			auto it = eventMap.find(a_event->eventName.c_str());
			if (it != eventMap.end())
			{
				spdlog::debug("Mod Event was called: {}", a_event->eventName.c_str());
				it->second(a_event); // If we found the key, we run the code that is attached to it.
			}
			else
				spdlog::debug("Mod Event not registered: {}", a_event->eventName.c_str());
		}

		return RE::BSEventNotifyControl::kContinue;
	}
};