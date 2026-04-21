#include "Utilities.h"
#include "Logger.h"
#include "Settings.h"
#include <windows.h>
#include <Xinput.h>
#pragma comment(lib, "Xinput.lib")

namespace ModernWaitMenu
{
	// Weather Manager
	void WeatherManager::updateCurrentWeather(RE::GFxMovieView* a_view, bool a_force)
	{
		RE::Sky* sky = RE::Sky::GetSingleton();
		if (sky && (a_force || sky->currentWeather))
		{
			// Retrieve the current weather
			RE::TESWeather* currentWeather = sky->currentWeather;

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
				MWM_LOG_WARN("Could not get Weather classification, returning 0");

			// Pack the information and send it to the menu.
			RE::GFxValue arg;
			arg.SetNumber(value);
			a_view->Invoke("_root.SleepWaitMenu_mc.updateWeather", nullptr, &arg, 1);
		}
	}

	// Time Manager
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

		// Pack all data.
		std::string s_minutes = fmt::format("{:02d}", minutes);
		std::string s_day = fmt::format("{:02d}", static_cast<int>(calendar->GetDay()));
		std::string s_dayName = calendar->GetDayName();
		std::string s_month = calendar->GetMonthName();

		const int size = 7;
		int index = 0;
		RE::GFxValue args[size];
		args[index++].SetNumber(hours12);
		args[index++].SetNumber(hours24);
		args[index++].SetString(s_minutes.c_str());
		args[index++].SetString(s_day.c_str());
		args[index++].SetString(s_dayName.c_str());
		args[index++].SetString(s_month.c_str());
		args[index++].SetNumber(calendar->GetYear());

		// Send the data to the menu
		if (size == index)
			a_view->Invoke("_root.SleepWaitMenu_mc.setTimeAndDate", nullptr, args, size);
		else
			MWM_LOG_CRITICAL("Argument count not correct! Size: {}; Index: {}", size, index);
	}

	// Control Manager
	void ControlManager::sendStickInformation(RE::GFxMovieView* a_view, const char* location, StickType stickType, float x, float y)
	{
		float& refLastX = (stickType == StickType::left ? lastLX : lastRX);
		float& refLastY = (stickType == StickType::left ? lastLY : lastRY);

		float deadzone = 0.25f;
		float magnitude = sqrt((x * x) + (y * y));

		if (magnitude < deadzone)
			x = y = 0.0f;

		// Only pack and send the data to the menu if we actually need to
		if (std::abs(x - refLastX) > 0.01f || std::abs(y - refLastY) > 0.01f)
		{
			RE::GFxValue args[2];
			args[0].SetNumber(x);
			args[1].SetNumber(y);

			a_view->Invoke(location, nullptr, args, 2);

			refLastX = x;
			refLastY = y;
		}
	}

	void ControlManager::sendDPadInformation(RE::GFxMovieView* a_view, const char* location, DPadType type)
	{
		bool up = type == DPadType::up;
		bool down = type == DPadType::down;
		bool left = type == DPadType::left;
		bool right = type == DPadType::right;

		bool anyPressed = (up || down || left || right);
		bool stateChanged = (up != lastUp || down != lastDown || left != lastLeft || right != lastRight);

		accumulator += RE::GetSecondsSinceLastFrame();
		bool sendData = false;

		if (stateChanged)
		{
			// If we pressed the key, this happens
			lastUp = up;
			lastDown = down;
			lastLeft = left;
			lastRight = right;

			if (anyPressed)
			{
				sendData = true;
				accumulator = 0.0;
			}
		}
		else if (anyPressed && accumulator >= Settings::DPadInitialDelay())
		{
			// If we hold the key for "DPadInitialDelay()" amount of seconds,
			// this will repeat until we let go of the key
			sendData = true;
			accumulator -= Settings::DPadRepeatRate();

			if (accumulator > Settings::DPadInitialDelay())
				accumulator = 0.0;
		}

		if (!anyPressed)
			accumulator = 0.0;

		// Only send the data if we really need to. This saves ressources and improves performance.
		if (sendData)
		{
			const int size = 4;
			RE::GFxValue args[size];
			args[0].SetBoolean(up);
			args[1].SetBoolean(down);
			args[2].SetBoolean(left);
			args[3].SetBoolean(right);

			a_view->Invoke(location, nullptr, args, size);
		}
	}
};