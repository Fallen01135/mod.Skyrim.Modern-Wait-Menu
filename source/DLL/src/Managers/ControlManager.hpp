// Copyright (c) 2026 Fallen01135. All rights reserved.
// Licensed under the Fallen01135 Mod License (see LICENSE.md).

#pragma once


#include "RE/G/GFxMovieView.h"
#include <array>


namespace ModernWaitMenu
{
	class ControlManager
	{
	private:
		// sendStickInformation
		inline static float lastLX = 0.0f, lastLY = 0.0f;
		inline static float lastRX = 0.0f, lastRY = 0.0f;

		// sendDPadInformation
		inline static std::array<bool, 4> states = { false, false, false, false };
		inline static std::array<bool, 4> lastStates = { false, false, false, false };
		inline static std::array<bool, 4> falseArray = { false, false, false, false }; // For easier comparison if any key is pressed
		inline static float accumulator = 0.0f;

	public:
		enum class StickType
		{
			left = 0,
			right = 1
		};

		enum class DPadType
		{
			up = 1,
			down = 2,
			left = 4,
			right = 8
		};

		/**
		* @brief Maps the parameter to the states bool array
		*
		* @param  id        The keycode that shall be mapped
		* @param  isDown    The value that the mapped key should get
		*/
		static void updateDPad(int id, bool isDown);

		/**
		* @brief This sends controller stick information to the Menu
		*
		* As AS2 does not have any real gamepad support, we need to make it.
		* For this, we gather the controller information from the standard library and send
		* it to the menu. We only send the information when the stick position was changed.
		*
		* @param  a_view      The Flash-Movie-Pointer of the Menu.
		* @param  location    This is the "path" inside the menu file to the function that should be called.
		* @param  left        If this should send the Left or Right stick information to the menu.
		*/
		static void sendStickInformation(RE::GFxMovieView* a_view, const char* location, StickType stickType, float x, float y);

		/**
		* @brief This sends gamepads D-Pad information to the Menu
		*
		* As AS2 does not have any real gamepad support, and the left stick is the same as the D-Pad so we make this here.
		* For this, we gather the controller information from the standard library and send
		* it to the menu. We only send the information when the D-Pad is used.
		*
		* @param  a_view      The Flash-Movie-Pointer of the Menu.
		* @param  location    This is the "path" inside the menu file to the function that should be called.
		*/
		static void sendDPadInformation(RE::GFxMovieView* a_view, const char* location);
	};
}