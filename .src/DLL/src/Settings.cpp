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
		ini.LoadFile(iniPath.c_str());

		std::apply
		(
			[&](auto&&... defs)
			{
				(
					[&](auto& def)
					{
						using T = std::decay_t<decltype(def.defaultValue)>;

						if constexpr (std::is_same_v<T, bool>)
						{
							def.value = ini.GetBoolValue(def.section, def.key, def.defaultValue);
							ini.SetBoolValue(def.section, def.key, def.value);
						}
						else if constexpr (std::is_same_v<T, int>)
						{
							def.value = static_cast<int>(ini.GetLongValue(def.section, def.key, def.defaultValue));
							ini.SetLongValue(def.section, def.key, static_cast<long>(def.value));
						}
						else if constexpr (std::is_same_v<T, float>)
						{
							def.value = static_cast<float>(ini.GetDoubleValue(def.section, def.key, def.defaultValue));
							ini.SetDoubleValue(def.section, def.key, static_cast<double>(def.value));
						}
						else if constexpr (std::is_same_v<T, std::string>)
						{
							def.value = std::string(ini.GetValue(def.section, def.key, def.defaultValue.c_str()));
							ini.SetValue(def.section, def.key, def.value.c_str());
						}

						MWM_LOG_INFO("Setting loaded: {} = {}", def.key, def.value);
					} (defs),
					...
				);
			},
			Data::ALL
		);

		ini.SaveFile(iniPath.c_str());

		applySettings();
	}

	void Settings::applySettings()
	{
		spdlog::set_level(getSetting(Data::bExtraLogging) ? spdlog::level::trace : spdlog::level::info);
		spdlog::flush_on(getSetting(Data::bExtraLogging) ? spdlog::level::trace : spdlog::level::info);

		_isVR = REL::Module::get().GetRuntime() == REL::Module::Runtime::VR;
	}
}