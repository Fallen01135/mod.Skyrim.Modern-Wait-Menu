#pragma once
#include <SimpleIni.h>


namespace ModernWaitMenu
{
	class Settings
	{
	private:
		// General
		inline static bool bUse24Clock{ true };
		inline static bool bUseLeadingZero{ true };

		// Controls
		inline static bool bActivateLeftStick{ true };
		inline static float fDPadInitialDelay{ 0.5f };
		inline static float fDPadRepeatRate{ 0.1f };

		// Debug
		inline static bool bExtraLogging{ false };

	public:
		// Getter
		[[nodiscard]] static bool is24Clock() noexcept { return bUse24Clock; };
		[[nodiscard]] static bool useLeadingZero() noexcept { return bUseLeadingZero; };

		[[nodiscard]] static bool leftStickActive() noexcept { return bActivateLeftStick; };
		[[nodiscard]] static float DPadInitialDelay() noexcept { return fDPadInitialDelay; };
		[[nodiscard]] static float DPadRepeatRate() noexcept { return fDPadRepeatRate; };

		[[nodiscard]] static bool extraLoggingActive() noexcept { return bExtraLogging; };

		/**
		* @brief This is the loader for the config ini file.
		*
		* It will load all values from the ini and then save them to the plugin in here.
		* If the ini cannot be loaded, it will use the default values.
		*/
		static void Load();

		/**
		* @brief This applies settings values that are important for the runtime
		*/
		static void applySettings();
	};
}