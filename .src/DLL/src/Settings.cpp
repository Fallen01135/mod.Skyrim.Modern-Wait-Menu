#include "Settings.h"
#include "Logger.h"


namespace ModernWaitMenu
{
	void Settings::Load()
	{
		// Retrieve the Plugin name and get the ini file.
		auto pluginName = SKSE::PluginDeclaration::GetSingleton()->GetName();
		std::string iniPath = std::format("Data/SKSE/Plugins/{}.ini", pluginName);

		CSimpleIniA ini;
		ini.SetUnicode();

		// Check if the ini file exists.
		SI_Error rc = ini.LoadFile(iniPath.c_str());
		if (rc < 0)
		{
			// If not, we throw an error
			MWM_LOG_WARN("Settings: No Settings found in {}, using defaults.", iniPath);
			return;
		}

		// Retrieve the values out of the ini, if the ini is not found we use the default value
		bUse24Clock = ini.GetBoolValue("General", "bUse24Clock", true);
		bUseLeadingZero = ini.GetBoolValue("General", "bUseLeadingZero", true);

		bActivateLeftStick = ini.GetBoolValue("Controls", "bActivateLeftStick", true);
		fDPadInitialDelay = static_cast<float>(ini.GetDoubleValue("Controls", "fDPadInitialDelay", 0.5));
		fDPadRepeatRate = static_cast<float>(ini.GetDoubleValue("Controls", "fDPadRepeatRate", 0.1));

		bExtraLogging = ini.GetBoolValue("Debug", "bExtraLogging", false);

		MWM_LOG_INFO("Settings: Loaded.");

		// Output all of the applied settings
		MWM_LOG_INFO("Use 24 Clock: {}", bUse24Clock);
		MWM_LOG_INFO("Use Leading Zero: {}", bUseLeadingZero);

		MWM_LOG_INFO("Left Stick active: {}", bActivateLeftStick);
		MWM_LOG_INFO("D-Pad Initial Delay: {}", fDPadInitialDelay);
		MWM_LOG_INFO("D-Pad Repeat Rate: {}", fDPadRepeatRate);

		MWM_LOG_INFO("Extra Logs: {}", bExtraLogging);

		applySettings();
	}

	void Settings::applySettings()
	{
		spdlog::set_level(bExtraLogging ? spdlog::level::trace : spdlog::level::info);
		spdlog::flush_on(bExtraLogging ? spdlog::level::trace : spdlog::level::info);

		_isVR = REL::Module::get().GetRuntime() == REL::Module::Runtime::VR;
	}
}