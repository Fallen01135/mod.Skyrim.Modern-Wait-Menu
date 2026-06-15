#pragma once


#include <SimpleIni.h>
#include <format>
#include <ranges>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <vector>


namespace SettingsLib
{
	/**
	* @brief All the load modes to use
	*
	* @param ReadOnly Only read values
	* @param ReadAndRepair Read all values and if the ini is missing a key, write it to the ini and save it
	* @param ReadAndWrite Read all the values and write them to the ini at the same time. This always tries to make the ini
	* an exact copy of the settings struct
	*/
	enum class LoadMode
	{
		ReadOnly,
		ReadAndRepair,
		ReadAndWrite
	};

	/**
	* @brief All the save modes to use
	*
	* @param StoreOnly Saves all values to the ini
	* @param StoreDefault Saves all default values to the ini file, this resets the ini.
	*/
	enum class SaveMode
	{
		StoreOnly,
		StoreDefault
	};

	/**
	* @brief All available comment style that can be used.
	*
	* @param Semicolon "; "
	* @param Hash "# "
	* @param None ""
	*/
	enum class CommentStyle
	{
		None,
		Semicolon,
		Hash
	};


	/**
	* @brief The definition item for each setting.
	*
	* @param sec The section this key sits in
	* @param key The name of the key
	* @param def The default value for this key
	* @param com A comment that will be placed above the key, can be empty
	* @param validator A lambda call that returns a boolean, used to check if a value inside the ini is correct, in range, etc... Can be empty.
	* Needs to return a boolean, if used.
	* @param hex This tells the Item if it is a Hex number or not. Does not work for float, doubles and strings. Can be empty.
	*/
	template <typename T>
	struct Item
	{
		struct Info
		{
			const char *section;
			const char *key;
			T defaultValue;
			const char *comment{nullptr};
			bool (*validator)(const T &value) { nullptr };
			bool isHex{false};
		} info;

		T value;

		Item
		(
			const char *sec,
			const char *key,
			T def,
			const char *com = nullptr,
			bool (*validator)(const T &value) = nullptr,
			bool hex = false
		)
		: info{sec, key, def, com, validator, hex}, value(def)
		{
		}

		operator T() const { return value; };
		auto operator[](size_t index) const requires requires { value[index]; } { return value[index]; };
	};


	// Only stuff used by the library to work.
	// Do not use anywhere else.
	namespace details
	{
		inline const char *_style = "; ";

		// Convertion from string to type.
		// This one is used for getting the values out of the ini
		template<typename T>
		inline T stringToType(const std::string &item)
		{
			// bool support
			if constexpr (std::is_same_v<T, bool>)
				return item == "true" || item == "1" || item == "on" || item == "yes";

			// signed number support
			else if constexpr (std::is_same_v<T, char>)
				return static_cast<char>(std::stoi(item, nullptr, 0));
			else if constexpr (std::is_same_v <T, signed char>)
				return static_cast<signed char>(std::stoi(item, nullptr, 0));
			else if constexpr (std::is_same_v<T, short>)
				return static_cast<short>(std::stoi(item, nullptr, 0));
			else if constexpr (std::is_same_v<T, int>)
				return std::stoi(item, nullptr, 0);
			else if constexpr (std::is_same_v<T, long>)
				return std::stol(item, nullptr, 0);
			else if constexpr (std::is_same_v<T, long long>)
				return std::stoll(item, nullptr, 0);

			// unsigned number support
			else if constexpr (std::is_same_v<T, unsigned char>)
				return static_cast<unsigned char>(std::stoul(item, nullptr, 0));
			else if constexpr (std::is_same_v<T, unsigned short>)
				return static_cast<unsigned short>(std::stoul(item, nullptr, 0));
			else if constexpr (std::is_same_v<T, unsigned int>)
				return static_cast<unsigned int>(std::stoul(item, nullptr, 0));
			else if constexpr (std::is_same_v<T, unsigned long>)
				return std::stoul(item, nullptr, 0);
			else if constexpr (std::is_same_v<T, unsigned long long>)
				return std::stoull(item, nullptr, 0);

			// floating point number support
			else if constexpr (std::is_same_v<T, float>)
				return std::stof(item);
			else if constexpr (std::is_same_v<T, double>)
				return std::stod(item);
			else if constexpr (std::is_same_v<T, long double>)
				return std::stold(item);

			// enum support
			else if constexpr (std::is_enum_v<T>)
				return static_cast<T>(std::stoi(item, nullptr, 0));

			// string support
			else if constexpr (std::is_same_v<T, std::string>)
				return item;

			// everything that is unsopported goes here
			else
				return T{};
		}

		// Convertion from type to string
		// This one is used for converting all the types to string so they can be saved into the ini.
		template<typename T>
		inline std::string typeToString(const T &value, bool isHex)
		{
			if constexpr (std::is_same_v<T, bool>) // boolean support
				return value ? "true" : "false";
			else if constexpr (std::is_enum_v<T>) // enum support
			{
				auto enumValue = static_cast<std::underlying_type_t<T>>(value);

				if (isHex)
					return std::format("0x{:X}", static_cast<unsigned long long>(enumValue)); // Hex support
				else
					return std::format("{}", enumValue);
			}
			else if constexpr (std::is_same_v<T, char> || std::is_same_v<T, signed char> || std::is_same_v<T, unsigned char>) // Char to number
				return std::format("{}", static_cast<int>(value));
			else if constexpr (std::is_arithmetic_v<T>)
			{
				if constexpr (!std::is_floating_point_v<T>)
					if (isHex)
						return std::format("0x{:X}", value); // Hex support

				return std::format("{}", value);
			}
			else
				return std::format("{}", value);
		}

		inline std::vector<std::string> split(const std::string &string, char delimiter)
		{
			std::vector<std::string> elements;
			auto split_view = string | std::views::split(delimiter);

			for (auto &&part : split_view)
			{
				std::string_view item{part.begin(), part.end()};

				size_t start = item.find_first_not_of(" \t");
				size_t end = item.find_last_not_of(" \t");

				if (start != std::string_view::npos && end != std::string_view::npos)
					elements.emplace_back(item.substr(start, end - start + 1));
			}

			return elements;
		}

		template<typename T>
		inline std::string join(const std::vector<T> &items, bool isHex)
		{
			if (items.empty())
				return "";

			auto formatted_view = items | std::views::transform
			(
				[isHex](const T &val)
				{
					return typeToString(val, isHex);
				}
			) | std::views::join_with(std::string_view(", "));

			return std::ranges::to<std::string>(formatted_view);
		}

		template<typename T_Data, typename F>
		inline void forEach(F &&func)
		{
			std::apply
			(
				[&](auto&&... defs)
				{
					(func(defs), ...);
				},
				T_Data::ALL
			);
		}

		inline std::string constructComment(const char *commentStyle, const char *comment)
		{
			if (comment && comment[0] != '\0')
				return std::format("{}{}", commentStyle, comment);

			return "";
		}

		template<typename T>
		inline void validate(Item<T> &def, bool &writeToFile)
		{
			if (def.info.validator && !def.info.validator(def.value))
			{
				def.value = def.info.defaultValue;
				writeToFile = true;
			}
		}

		// This will get a value from the ini
		template<typename T>
		inline T getValue(CSimpleIniA &ini, Item<T> &def)
		{
			const char *raw = ini.GetValue(def.info.section, def.info.key, nullptr);

			if (raw)
				return stringToType<T>(std::string(raw));

			return def.info.defaultValue;
		}

		// This will save a value to the ini
		template<typename T>
		inline void setValue(CSimpleIniA &ini, Item<T> &def, const char *comment, bool &defaultMode)
		{
			T value = defaultMode ? def.info.defaultValue : def.value;
			std::string valueStr = typeToString<T>(value, def.info.isHex);

			ini.SetValue(def.info.section, def.info.key, valueStr.c_str(), comment);
		}


		#pragma region manageValue
		template<typename T>
		inline void manageValue(CSimpleIniA &ini, Item<T> &def, const char *comment, bool readFile, bool &writeToFile, bool defaultMode)
		{
			if (readFile)
				def.value = getValue<T>(ini, def);

			// We validate no matter what, if the current value is not correct, we default it.
			validate(def, writeToFile);

			if (writeToFile)
				setValue<T>(ini, def, comment, defaultMode);
		}

		template<typename T>
		inline void manageValue(CSimpleIniA &ini, Item<std::vector<T>> &def, const char *comment, bool readFile, bool &writeToFile, bool defaultMode)
		{
			if (readFile)
			{
				const char *raw = ini.GetValue(def.info.section, def.info.key, nullptr);
				if (raw)
				{
					def.value = split(raw, ',')
						| std::views::transform([](const std::string &item) { return stringToType<T>(item); })
						| std::ranges::to<std::vector>();
				}
				else
					def.value = def.info.defaultValue;
			}

			// We validate no matter what, if a current value is not correct, we default it.
			validate(def, writeToFile);

			if (writeToFile)
			{
				bool isHex = def.info.isHex;
				ini.SetValue(def.info.section, def.info.key, defaultMode ? join(def.info.defaultValue, isHex).c_str() : join(def.value, isHex).c_str(), comment);
			}
		}
		#pragma endregion

		template<typename T, typename Mode>
		inline bool manage(CSimpleIniA &ini, Item<T> &def, Mode mode)
		{
			bool readOnly = false, readAndRepair = false, readAndWrite = false, storeOnly = false, storeDefault = false;

			if constexpr (std::is_same_v<Mode, LoadMode>)
			{
				readOnly = (mode == LoadMode::ReadOnly);
				readAndRepair = (mode == LoadMode::ReadAndRepair);
				readAndWrite = (mode == LoadMode::ReadAndWrite);
			}
			else if constexpr (std::is_same_v<Mode, SaveMode>)
			{
				storeOnly = (mode == SaveMode::StoreOnly);
				storeDefault = (mode == SaveMode::StoreDefault);
			}

			bool writeToFile = (readAndRepair && !ini.KeyExists(def.info.section, def.info.key))
				|| readAndWrite
				|| storeOnly
				|| storeDefault;

			bool readFile = !storeOnly && !storeDefault;

			// When we write to the file, we make sure the ini is an exact copy of our settings
			if (writeToFile)
				ini.Delete(def.info.section, def.info.key, false);

			std::string comment = constructComment(_style, def.info.comment);
			const char *commentPtr = comment.empty() ? nullptr : comment.c_str();

			manageValue(ini, def, commentPtr, readFile, writeToFile, storeDefault);

			// Check, so we never write to the file when we only want to read
			// Important as we have our validator, and this one sets writeToFile to true when finding a problem.
			if (readOnly)
				writeToFile = false;

			return writeToFile;
		}
	}


	// ==================
	//  -> Public API <-
	// ==================

	/**
	* @brief This allows to change the comment style.
	*
	* Allows for switching between ";", "#" and none.
	* If none was selected, the user needs to add them to their comments.
	*
	* @param style The CommentStyle to switch to.
	*/
	inline void SetCommentStyle(CommentStyle style)
	{
		switch (style)
		{
			case CommentStyle::Semicolon:
				details::_style = "; ";
				break;
			case CommentStyle::Hash:
				details::_style = "# ";
				break;
			case CommentStyle::None:
				details::_style = "";
				break;
			default:
				details::_style = "; ";
				break;
		}
	}

	/**
	* @brief This loads all the settings defined in the users settings struct
	*
	* This fills the settings struct defined by the user with all the current values
	* inside the ini file.
	*
	* @param iniPath The path to the ini file.
	* @param mode The LoadMode to load the file with. Default: ReadAndRepair
	*
	* @return Boolean: If the ini was found or not.
	*/
	template<typename T_Data>
	static bool Load(const std::string &iniPath, LoadMode mode = LoadMode::ReadAndRepair)
	{
		CSimpleIniA ini;
		ini.SetUnicode();

		SI_Error err = ini.LoadFile(iniPath.c_str());
		if (err < 0 && mode == LoadMode::ReadOnly)
			return false;

		bool saveFile = false;
		details::forEach<T_Data>
			(
				[&](auto &setting)
				{
					if (details::manage(ini, setting, mode))
						saveFile = true;
				}
			);

		if (saveFile)
			ini.SaveFile(iniPath.c_str());

		return true;
	}

	/**
	* @brief This allows to save the users settings struct to the ini file
	*
	* If the settings struct was change in code, this can be used to save the changed
	* values to the ini.
	*
	* @param iniPath The path to the ini file.
	* @param mode The SaveMode to save the file with. Default: StoreOnly
	*/
	template<typename T_Data>
	static void Save(const std::string &iniPath, SaveMode mode = SaveMode::StoreOnly)
	{
		CSimpleIniA ini;
		ini.SetUnicode();
		ini.LoadFile(iniPath.c_str());

		details::forEach<T_Data>
			(
				[&](auto &setting)
				{
					details::manage(ini, setting, mode);
				}
			);

		ini.SaveFile(iniPath.c_str());
	}

	template<typename T_Data, typename F>
	static void Dump(F &&callback)
	{
		details::forEach<T_Data>
			(
				[&](const auto &setting)
				{
					callback(setting);
				}
			);
	}
}

// So that std::format can actually work with the data types declared in here
template<typename T>
struct std::formatter<SettingsLib::Item<T>> : std::formatter<T>
{
	auto format(const SettingsLib::Item<T> &item, format_context &ctx) const
	{
		return std::formatter<T>::format(item.value, ctx);
	}
};