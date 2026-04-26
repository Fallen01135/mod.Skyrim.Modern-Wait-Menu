import os

settings = []
with open('settings.list', 'r') as f:
	for line in f:
		if line.startswith('#') or not line.strip(): continue
		parts = [p.strip() for p in line.split('|')]
		settings.append({'type': parts[0], 'name': parts[1], 'section': parts[2], 'default': parts[3]})

items_code = ""
tuple_entries = []

for s in settings:
	items_code += f'			inline static Item<{s["type"]}> {s["name"]}{{ "{s["section"]}", "{s["name"]}", {s["default"]}, {s["default"]} }};\n'
	tuple_entries.append(f'				std::ref({s["name"]})')

tuple_code = ",\n".join(tuple_entries)

header_content = f"""#pragma once
#include "Logger.h"
#include <SimpleIni.h>


namespace ModernWaitMenu
{{
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
	{{
	private:
		template <typename T> struct Item
		{{
			const char* section;
			const char* key;
			T defaultValue;
			mutable T value;
		}};

		inline static bool _isVR{{ false }};

	public:
		struct Data
		{{
{items_code}
			static constexpr auto ALL = std::make_tuple
			(
{tuple_code}
			);
		}};

		#pragma region Getters
		// Getter for easier access
		template <typename T> [[nodiscard]] static T getSetting(const Item<T>& setting)
		{{
			return setting.value;
		}};

		// Other
		[[nodiscard]] static bool isVR() noexcept {{ return _isVR; }};
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
	}};
}}
"""

with open('Settings.h', 'w', encoding='utf-8') as f:
	f.write(header_content)

print("Settings.h was generated!")
input("Press Enter to close")