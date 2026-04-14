#pragma once
#include <SimpleIni.h>

namespace ModernWaitMenu
{
	struct Settings
	{
		// General
		bool bUse24Clock{true};
		bool bUseLeadingZero{true};

		// Debug
		bool bExtraLogging{ false };

		static Settings* GetSingleton()
		{
			static Settings instance;
			return &instance;
		}

		/**
		* @brief This is the loader for the config ini file.
		*
		* It will load all values from the ini and then save them to the plugin in here.
		* If the ini cannot be loaded, it will use the default values.
		*/
		void Load();

		/**
		* @brief This applies settings values that are important for the runtime
		*/
		void applySettings();
	};
}