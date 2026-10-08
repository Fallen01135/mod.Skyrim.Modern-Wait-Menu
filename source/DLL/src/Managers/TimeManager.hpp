// Copyright (c) 2026 Fallen01135. All rights reserved.
// Licensed under the Fallen01135 Mod License (see LICENSE.md).

#pragma once


#include "RE/G/GFxMovieView.h"
#include <string>


namespace ModernWaitMenu
{
	class TimeManager
	{
	private:
		inline static int lastHours = -1;
		inline static int lastMinutes = -1;

		static void ReplaceInText(std::string& text, const std::string& search, const std::string& replace);

	public:
		/**
		* @brief Updates the Time and date of the Wait Menu.
		*
		* It will send the collected informations into the Wait menu.
		*
		* @param  a_view     The Flash-Movie-Pointer of the Menu.
		* @param  a_force    If true, the update will be forced even without any Time change.
		*/
		static void UpdateMenuTime(RE::GFxMovieView* a_view, bool a_force);
	};
}