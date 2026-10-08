// Copyright (c) 2026 Fallen01135. All rights reserved.
// Licensed under the Fallen01135 Mod License (see LICENSE.md).

#pragma once


#include "RE/B/BSTEvent.h"
#include "RE/I/InputEvent.h"


namespace ModernWaitMenu
{
	class InputEventSink :
		public RE::BSTEventSink<RE::InputEvent *>
	{
		private:

		public:
		static InputEventSink*GetSingleton()
		{
			static InputEventSink instance;
			return &instance;
		}

		/**
		* @brief Handles the input events.
		*
		* On pressing a keyboard or gamepad key, this event is fired.
		* We then check if the Left-Stick is used and if we start sending the information to the menu
		* If one of the D-Pad keys were used, we send the information to the menu as long as we hold it.
		*
		* @param  a_event          The event data containing all informations about the input.
		* @param  a_eventSource    The event source that dispatched this event.
		*
		* @return                  RE::BSEventNotifyControl::kContinue to allow other plugins to receive this event.
		*/
		RE::BSEventNotifyControl ProcessEvent(RE::InputEvent *const *a_event, RE::BSTEventSource<RE::InputEvent *> *a_eventSource) override;
	};
};