#include "Settings.h"
#include "Logger.h"
#include "Utilities.h"


namespace ModernWaitMenu
{
	// Weather Manager
	void WeatherManager::updateCurrentWeather(RE::GFxMovieView* a_view, bool a_force)
	{
		// Retrieve the current weather
		auto sky = RE::Sky::GetSingleton();
		auto currentWeather = sky->currentWeather;
		if (!sky || !currentWeather)
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
			MWM_LOG_WARN("Could not get Weather classification, returning 0");

		// Pack the information and send it to the menu.
		RE::GFxValue arg;
		arg.SetNumber(value);
		a_view->Invoke("_root.SleepWaitMenu_mc.updateWeather", nullptr, &arg, 1);
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
	void ControlManager::updateDPad(int id, bool isDown)
	{
		int i = -1;
		auto idNew = static_cast<DPadType>(id);
		switch (idNew)
		{
			case DPadType::up : i = 0; break;
			case DPadType::down : i = 1; break;
			case DPadType::left : i = 2; break;
			case DPadType::right : i = 3; break;
			default:
				return;
		}

		if (states[i] != isDown)
		{
			states[i] = isDown;
			MWM_LOG_DEBUG("D-Pad state changed: {} is now {}", i, isDown);
		}
	}

	void ControlManager::sendStickInformation(RE::GFxMovieView* a_view, const char* location, StickType stickType, float x, float y)
	{
		float& refLastX = (stickType == StickType::left ? lastLX : lastRX);
		float& refLastY = (stickType == StickType::left ? lastLY : lastRY);

		float deadzone = 0.25f;
		float magnitude = (x * x) + (y * y);

		if (magnitude < (deadzone * deadzone))
			x = y = 0.0f;

		// Only pack and send the data to the menu if we actually need to
		// This lastX and lastY logic is not really neccesary, but it might save some ressources.
		// Especially if the player is holding the position and is not letting it go, as the event would still keep firing.
		// Also it might prevent stick drift. And it makes it able to be included in other scenarios.
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

	void ControlManager::sendDPadInformation(RE::GFxMovieView* a_view, const char* location)
	{
		// Check if something was pressed, if not we just quite
		bool anyPressed = states != falseArray;
		bool stateChanged = states != lastStates;

		accumulator += RE::GetSecondsSinceLastFrame();

		bool sendData = false;
		if (stateChanged)
		{
			lastStates = states;

			sendData = true;
			accumulator = 0.0f;
		}
		else if (anyPressed && accumulator >= Settings::getSetting(Settings::Data::fDPadInitialDelay))
		{
			// If we hold the key for "DPadInitialDelay()" amount of seconds,
			// this will repeat until we let go of the key
			sendData = true;

			accumulator -= Settings::getSetting(Settings::Data::fDPadRepeatRate);

			if (accumulator > Settings::getSetting(Settings::Data::fDPadInitialDelay))
				accumulator = 0.0f;
		}

		if (!anyPressed)
		{
			accumulator = 0.0f;
		}

		// Only send the data if we really need to. This saves ressources and improves performance.
		if (sendData)
		{
			const int size = 4;
			RE::GFxValue args[size];
			for (size_t i = 0; i < size; i++)
				args[i].SetBoolean(states[i]);

			a_view->Invoke(location, nullptr, args, size);
		}
	}
};