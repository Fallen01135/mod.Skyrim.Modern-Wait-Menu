// Copyright (c) 2026 Fallen01135. All rights reserved.
// Licensed under the Fallen01135 Mod License (see LICENSE.md).

#include "RE/G/GFxMovieView.h"
#include "RE/G/GFxValue.h"
#include "RE/S/Sky.h"
#include "WeatherManager.hpp"
#include "spdlog/spdlog.h"
#include <cmath>


namespace ModernWaitMenu
{
	void WeatherManager::updateCurrentWeather(RE::GFxMovieView* a_view, bool a_force)
	{
		// Retrieve the current sky
		auto sky = RE::Sky::GetSingleton();
		if (!sky)
			return;

		// Retrieve the current weather
		auto currentWeather = sky->currentWeather;
		if (!currentWeather)
			return;

		/*
			Since this can/will be called each frame of the menu, we make sure we only proceed if we need to.
			So we do not send information to the menu on each frame, and only when it actually changes or we force it.
			This is done as we do not want to call the invoke function every frame of the menu, as this is not a good
			idea and extremly risky and not really performant.
			All of those checks makes sure we only fire the invoke call only once and only when we need it.
		*/
		if (!a_force && currentWeather == lastWeather)
			return;
		lastWeather = currentWeather;

		/*
			Retrieve the weather classification.
			As the flags are translations of bits, we need to bring it into the power of what they represent.
			It will turn it like:
				log_2(1) -> 0
				log_2(2) -> 1
				log_2(4) -> 2
				log_2(8) -> 3
			For this we use the logarithm to the base of 2 function.
			It does the opposite of taking the number 2 to a power of n, like 2^n
			2^2 = 4 | log_2(4) = 2
			Therefore we also make sure, that the value of "flag" is greater then 0.
		*/
		int flag = static_cast<int>(currentWeather->data.flags.get());
		int value = (flag > 0 && flag <= 8) ? static_cast<int>(std::log2(flag)) : 0;
		if (flag <= 0 || flag > 8)
			spdlog::warn("Could not get Weather classification, returning 0");

		// Pack the information and send it to the menu.
		RE::GFxValue arg;
		arg.SetNumber(value);
		a_view->Invoke("_root.SleepWaitMenu_mc.updateWeather", nullptr, &arg, 1);
	}
}