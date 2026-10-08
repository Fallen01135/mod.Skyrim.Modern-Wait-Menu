// Copyright (c) 2026 Fallen01135. All rights reserved.
// Licensed under the Fallen01135 Mod License (see LICENSE.md).

#pragma once


#include "RE/G/GFxMovieView.h"
#include "RE/T/TESWeather.h"


namespace ModernWaitMenu
{
	class WeatherManager
	{
	private:
		// For me: inline static is technically the same as in Java static
		inline static RE::TESWeather* lastWeather = nullptr;

	public:
		/**
		* @brief Updates the Weather of the Wait Menu.
		*
		* It will send the collected informations into the Wait menu.
		*
		* @param  a_view     The Flash-Movie-Pointer of the Menu.
		* @param  a_force    If true, the update will be forced even without any weather change.
		*/
		static void updateCurrentWeather(RE::GFxMovieView* a_view, bool a_force);
	};
}