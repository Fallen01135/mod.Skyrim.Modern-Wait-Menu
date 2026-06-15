#pragma once


#include "../include/SimpleSettings.hpp"
#include <functional>
#include <string>
#include <tuple>
#include <vector>


/**
* GENERATED CODE - DO NOT MODIFY MANUALLY.
* Source: settings.tsv
*/
namespace ModernWaitMenu
{
	struct Settings
	{
		inline static SettingsLib::Item<bool> bUse24Clock
		{
		    "General",
		    "bUse24Clock",
		    true,
		    "If the 24 hours clock should be used or the 12 hours clock"
		};

		inline static SettingsLib::Item<bool> bUseLeadingZero
		{
		    "General",
		    "bUseLeadingZero",
		    true,
		    "If a zero should be added to every number if they are not having 2 digits. Only works for the time, not the date."
		};

		inline static SettingsLib::Item<bool> bUseCustomCursor
		{
		    "General",
		    "bUseCustomCursor",
		    true,
		    "If you want to use the custom cursor when hovering over the area"
		};

		inline static SettingsLib::Item<std::string> sDateString
		{
		    "General",
		    "sDateString",
		    "{DD}, {dd}. {MM} {YY}",
		    "This is the date string found ingame. For more information check: https://github.com/users/Fallen01135/projects/3?pane=issue&itemId=200072401"
		};

		inline static SettingsLib::Item<bool> bActivateLeftStick
		{
		    "Controls",
		    "bActivateLeftStick",
		    true,
		    "If the left stick should be able to be used to select the time"
		};

		inline static SettingsLib::Item<float> fDPadInitialDelay
		{
		    "Controls",
		    "fDPadInitialDelay",
		    0.5f,
		    "The time someone needs to hold the button before the time start scrolling"
		};

		inline static SettingsLib::Item<float> fDPadRepeatRate
		{
		    "Controls",
		    "fDPadRepeatRate",
		    0.1f,
		    "How fast the time should scroll when the button is held"
		};

		inline static SettingsLib::Item<bool> bExtraLogging
		{
		    "Debug",
		    "bExtraLogging",
		    false,
		    "If additional logging should be used"
		};


		static constexpr auto ALL = std::make_tuple
		(
			std::ref(bUse24Clock),
			std::ref(bUseLeadingZero),
			std::ref(bUseCustomCursor),
			std::ref(sDateString),
			std::ref(bActivateLeftStick),
			std::ref(fDPadInitialDelay),
			std::ref(fDPadRepeatRate),
			std::ref(bExtraLogging)
		);
	};
}
