// Copyright (c) 2026 Fallen01135. All rights reserved.
// Licensed under the Fallen01135 Mod License (see LICENSE.md).

#pragma once


#include "RE/B/BSTEvent.h"
#include "RE/G/GFxMovieView.h"
#include "RE/I/InputEvent.h"
#include "RE/M/MenuOpenCloseEvent.h"
#include "SKSE/Events.h"
#include <string_view>


namespace ModernWaitMenu
{
	/**
	* @brief Here are all the event listeners declared which are used by this plugin
	*/
	class MenuEventSink :
		public RE::BSTEventSink<RE::MenuOpenCloseEvent>
	{
	private:
		inline static constexpr std::string_view as2VarNames[] = { "suffixAM", "suffixPM", "useLeadingZero", "is24Clock", "useCustomCursor" };

	public:
		static MenuEventSink* GetSingleton()
		{
			static MenuEventSink instance;
			return &instance;
		}

		/**
		* @brief Handles the menu open and close events for the SleepWaitMenu.
		*
		* When the menu opens this will initialize the menu with all the important weather, time and date information.
		*
		* @param  a_event          The event data containing all informations about the menu.
		* @param  a_eventSource    The event source that dispatched this event.
		*
		* @return                  RE::BSEventNotifyControl::kContinue to allow other plugins to receive this event.
		*/
		RE::BSEventNotifyControl ProcessEvent(const RE::MenuOpenCloseEvent *a_event, RE::BSTEventSource<RE::MenuOpenCloseEvent> *a_eventSource) override;
	};
};