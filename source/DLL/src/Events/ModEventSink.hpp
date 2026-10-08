// Copyright (c) 2026 Fallen01135. All rights reserved.
// Licensed under the Fallen01135 Mod License (see LICENSE.md).

#pragma once


#include "RE/B/BSTEvent.h"
#include "SKSE/Events.h"


namespace ModernWaitMenu
{
	/**
	* @brief Here are all the event listeners declared which are used by this plugin
	*/
	class ModEventSink :
		public RE::BSTEventSink<SKSE::ModCallbackEvent>
	{
	private:

	public:
		static ModEventSink* GetSingleton()
		{
			static ModEventSink instance;
			return &instance;
		}

		/**
		* @brief Handles the SKSE mod callback events.
		*
		* On retrieving a mod event, we will check inside our eventMap if we have one that matches the string recieved from
		* the mod event. If so we run the code attached to it.
		*
		* @param  a_event          The event data containing all informations about the Mod Event.
		* @param  a_eventSource    The event source that dispatched this event.
		*
		* @return                  RE::BSEventNotifyControl::kContinue to allow other plugins to receive this event.
		*/
		RE::BSEventNotifyControl ProcessEvent(const SKSE::ModCallbackEvent *a_event, RE::BSTEventSource<SKSE::ModCallbackEvent> *a_eventSource) override;
	};
};