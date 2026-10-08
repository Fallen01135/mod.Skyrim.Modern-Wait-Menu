// Copyright (c) 2026 Fallen01135. All rights reserved.
// Licensed under the Fallen01135 Mod License (see LICENSE.md).

#include "RE/C/Calendar.h"
#include "RE/G/GFxMovieView.h"
#include "RE/G/GFxValue.h"
#include "Settings.hpp"
#include "TimeManager.hpp"
#include "spdlog/spdlog.h"
#include <format>
#include <string>


namespace ModernWaitMenu
{
	void TimeManager::UpdateMenuTime(RE::GFxMovieView* a_view, bool a_force)
	{
		// Retrieve the calender which holds all information about time and date.
		RE::Calendar* calendar = RE::Calendar::GetSingleton();
		if (!calendar || !calendar->gameHour)
			return;

		// Get the current time float and convert it to a more useable format
		float gameHours = calendar->gameHour->value;
		int hours24 = static_cast<int>(gameHours);

		float fraction = gameHours - static_cast<float>(hours24);
		int minutes = static_cast<int>(fraction * 60.0f);

		if (minutes >= 60)
			minutes = 59;

		/*
			Since this can/will be called each frame of the menu, we make sure we only proceed if we need to.
			So we do not send information to the menu on each frame, and only when it actually changes or we force it.
			This is done as we do not want to call the invoke function every frame of the menu, as this is not a good
			idea and extremly risky and not really performant.
			All of those checks makes sure we only fire the invoke call only once and only when we need it.
		*/
		RE::GFxValue bWaitingValue;
		bool isWaiting = false;
		if (a_view->GetVariable(&bWaitingValue, "_root.SleepWaitMenu_mc.isWaiting"))
			isWaiting = bWaitingValue.GetBool();

		if (!a_force)
		{
			if (isWaiting)
			{
				if (hours24 == lastHours)
					return;
			}
			else
			{
				if (hours24 == lastHours && minutes == lastMinutes)
					return;
			}
		}

		lastHours = hours24;
		lastMinutes = minutes;

		// Convert the 24 hours with the modulo of 12.
		int hours12 = ((hours24 % 12) == 0) ? 12 : (hours24 % 12);

		// Date string text replacement
		std::string dateString = Settings::sDateString;

		int dayNumber = static_cast<int>(calendar->GetDay());
		int monthNumber = static_cast<int>(calendar->GetMonth()) + 1;
		int yearNumber = static_cast<int>(calendar->GetYear());

		std::string dayName = calendar->GetDayName();
		std::string monthName = calendar->GetMonthName();

		ReplaceInText(dateString, "{d}", std::to_string(dayNumber));
		ReplaceInText(dateString, "{dd}", std::format("{:02d}", dayNumber));
		ReplaceInText(dateString, "{DD}", dayName);

		ReplaceInText(dateString, "{m}", std::to_string(monthNumber));
		ReplaceInText(dateString, "{mm}", std::format("{:02d}", monthNumber));
		ReplaceInText(dateString, "{MM}", monthName);

		ReplaceInText(dateString, "{yy}", std::to_string(yearNumber));
		ReplaceInText(dateString, "{YY}", std::format("4E {}", yearNumber));

		spdlog::debug("New date string: {}" + dateString);

		// Pack all data.
		std::string s_minutes = std::format("{:02d}", minutes);

		const int size = 4;
		int index = 0;
		RE::GFxValue args[size];
		args[index++].SetNumber(hours12);
		args[index++].SetNumber(hours24);
		args[index++].SetString(s_minutes);
		args[index++].SetString(dateString);

		// Send the data to the menu
		if (size == index)
			a_view->Invoke("_root.SleepWaitMenu_mc.setTimeAndDate", nullptr, args, size);
		else
			spdlog::critical("Argument count not correct! Size: {}; Index: {}", size, index);
	}

	void TimeManager::ReplaceInText(std::string& text, const std::string& search, const std::string& replace)
	{
		size_t pos;
		while ((pos = text.find(search)) != std::string::npos)
			text.replace(pos, search.length(), replace);
	}
}