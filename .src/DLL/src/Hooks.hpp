#pragma once


namespace ModernWaitMenu
{
	class SleepWaitMenuHook
	{
	private:
		using ProcessMessage_t = RE::UI_MESSAGE_RESULTS(RE::SleepWaitMenu*, RE::UIMessage&);
		static inline REL::Relocation<ProcessMessage_t> _ProcessMessage;

		/**
		* @brief This will update the menu on each frame of it.
		*
		* It sends the menu all important information about weather, time and date as long as it is opened
		* and running. This is done for each frame of the menu.
		*
		* @param a_this This is the current instance of the SleepWaitMenu.
		* @param a_message The message object which explains all the current UI-Events (Update, Hotkeys, etc..)
		* 
		* @return The result. Was our message accepted?
		*/
		static RE::UI_MESSAGE_RESULTS ProcessMessage_Hook(RE::SleepWaitMenu* a_this, RE::UIMessage& a_message);

	public:
		/**
		* (No quaranty that all of this is correct)
		* @brief This installs the Hook for the SleepWaitMenu through overwriting the Virtual Function Table (VTable).
		* This will localise the VTable of the menu in the memory of the game and replaces the original pointer
		* with our own "ProcessMessage_Hook". The original function will be stored inside "_ProcessMessage" to be
		* able to use it later inside of the Hook.
		*/
		static void Install();
	};
}