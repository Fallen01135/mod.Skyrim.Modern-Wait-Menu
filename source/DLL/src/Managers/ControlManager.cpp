// Copyright (c) 2026 Fallen01135. All rights reserved.
// Licensed under the Fallen01135 Mod License (see LICENSE.md).

#include "../Platform/Offset.hpp"
#include "ControlManager.hpp"
#include "RE/G/GFxMovieView.h"
#include "RE/G/GFxValue.h"
#include "Settings.hpp"
#include "spdlog/spdlog.h"
#include <cstdlib>


namespace ModernWaitMenu
{
	void ControlManager::updateDPad(int id, bool isDown)
	{
		int i = -1;
		auto idNew = static_cast<DPadType>(id);
		switch (idNew)
		{
		case DPadType::up: i = 0; break;
		case DPadType::down: i = 1; break;
		case DPadType::left: i = 2; break;
		case DPadType::right: i = 3; break;
		default:
			return;
		}

		if (states[i] != isDown)
		{
			states[i] = isDown;
			spdlog::debug("D-Pad state changed: {} is now {}", i, isDown);
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
		bool anyPressed = states != falseArray;
		bool stateChanged = states != lastStates;

		float seconds = *REL::Relocation<float*>(Offset::Time::FrameTimer.address()).get();
		accumulator += seconds;

		bool sendData = false;
		if (stateChanged)
		{
			lastStates = states;

			sendData = true;
			accumulator = 0.0f;
		}
		else if (anyPressed && accumulator >= Settings::fDPadInitialDelay)
		{
			// If we hold the key for "DPadInitialDelay()" amount of seconds,
			// this will repeat until we let go of the key
			sendData = true;

			accumulator -= Settings::fDPadRepeatRate;

			if (accumulator > Settings::fDPadInitialDelay)
				accumulator = 0.0f;
		}

		if (!anyPressed)
			accumulator = 0.0f;

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
}