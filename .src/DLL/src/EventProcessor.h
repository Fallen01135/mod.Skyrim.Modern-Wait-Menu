#pragma once

namespace ModernWaitMenu
{
	/**
	* @brief Here are all the event listeners declared which are used by this plugin
	*/
	class EventProcessor :
		public RE::BSTEventSink<RE::MenuOpenCloseEvent>,
		public RE::BSTEventSink<SKSE::ModCallbackEvent>,
		public RE::BSTEventSink<RE::InputEvent*>
	{
	private:
		inline static constexpr std::string_view as2VarNames[] = { "suffixAM", "suffixPM", "useLeadingZero", "is24Clock" };
		inline static RE::GFxMovieView* view { nullptr };

	public:
		static EventProcessor* GetSingleton()
		{
			static EventProcessor instance;
			return &instance;
		}

		/**
		* @brief Handles the menu open and close events for the SleepWaitMenu.
		* 
		* When the menu opens this will initialize the menu with all the important weather, time and date information.
		* 
		* @param a_event The event data containing all informations about the menu.
		* @param a_eventSource The event source that dispatched this event.
		* 
		* @return RE::BSEventNotifyControl::kContinue to allow other plugins to receive this event.
		*/
		RE::BSEventNotifyControl ProcessEvent(const RE::MenuOpenCloseEvent* a_event, RE::BSTEventSource<RE::MenuOpenCloseEvent>* a_eventSource) override;

		/**
		* @brief Handles the SKSE mod callback events.
		* 
		* On retrieving a mod event, we will check inside our eventMap if we have one that matches the string recieved from
		* the mod event. If so we run the code attached to it.
		* 
		* @param a_event The event data containing all informations about the Mod Event.
		* @param a_eventSource The event source that dispatched this event.
		* 
		* @return RE::BSEventNotifyControl::kContinue to allow other plugins to receive this event.
		*/
		RE::BSEventNotifyControl ProcessEvent(const SKSE::ModCallbackEvent* a_event, RE::BSTEventSource<SKSE::ModCallbackEvent>* a_eventSource) override;

		RE::BSEventNotifyControl ProcessEvent(RE::InputEvent* const* a_event, RE::BSTEventSource<RE::InputEvent*>* a_eventSource) override;
	};
};