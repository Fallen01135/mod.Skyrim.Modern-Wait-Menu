// Copyright (c) 2026 Fallen01135. All rights reserved.
// Licensed under the Fallen01135 Mod License (see LICENSE.md).

#pragma once


namespace ModernWaitMenu
{
	class SettingsManager
	{
	private:
		inline static bool _isVR{ false };

	public:
		// getter/setter
		[[nodiscard]] static bool isVR() noexcept { return _isVR; };

		// Other
		static void Load();
		static void applySettings();
	};
}