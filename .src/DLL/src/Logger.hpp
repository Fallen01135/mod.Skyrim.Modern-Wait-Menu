#pragma once
#include <spdlog/spdlog.h>
#include <source_location>


namespace ModernWaitMenu
{
	class Logger
	{
	private:
	public:
		/**
		* @brief This initializes the custom logging function and creates a custom log file
		*/
		static void Init();
	};
}