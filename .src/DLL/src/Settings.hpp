#pragma once
#include "Logger.h"
#include <SimpleIni.h>


namespace ModernWaitMenu
{
	/**
	* @brief Manages all settings related actions.
	* * GENERATED CODE - DO NOT MODIFY MANUALLY.
	* 
	* This Settings class might be overkill for this single dll, I wanted this to be
	* build modular so I can simply copy paste this into other projects.
	* At the same time this Settings file is a test bench for me, so I can test out things,
	* as well as learning stuff.
	* 
	* This is why this might be way to complicated for this plugin alone.
	* But I wanted to have an approach that is modular for all kinds of tasks.
	*/
	class Settings
	{
	private:
		template <typename T> struct Item
		{
			const char* section;
			const char* key;
			T defaultValue;
			mutable T value;
		};

		inline static bool _isVR{ false };

	public:
		struct Data
		{
			inline static Item<bool> bUse24Clock{ "General", "bUse24Clock", true, true };
			inline static Item<bool> bUseLeadingZero{ "General", "bUseLeadingZero", true, true };
			inline static Item<bool> bActivateLeftStick{ "Controls", "bActivateLeftStick", true, true };
			inline static Item<float> fDPadInitialDelay{ "Controls", "fDPadInitialDelay", 0.5f, 0.5f };
			inline static Item<float> fDPadRepeatRate{ "Controls", "fDPadRepeatRate", 0.1f, 0.1f };
			inline static Item<bool> bExtraLogging{ "Debug", "bExtraLogging", false, false };

			static constexpr auto ALL = std::make_tuple
			(
				std::ref(bUse24Clock),
				std::ref(bUseLeadingZero),
				std::ref(bActivateLeftStick),
				std::ref(fDPadInitialDelay),
				std::ref(fDPadRepeatRate),
				std::ref(bExtraLogging)
			);
		};

		#pragma region Getters
		// Getter for easier access
		template <typename T> [[nodiscard]] static T getSetting(const Item<T>& setting)
		{
			return setting.value;
		};

		// Other
		[[nodiscard]] static bool isVR() noexcept { return _isVR; };
		#pragma endregion


		/**
		* @brief This is the loader for the config ini file.
		*
		* This will load all definied ini settings we defined in the Data struct.
		* It then retrieves the value for inside the ini, and fils the value with it.
		* Afterwards it can be easily used on other places.
		*/
		static void Load();

		/**
		* @brief This applies settings values that are important for the runtime
		*/
		static void applySettings();
	};
}
