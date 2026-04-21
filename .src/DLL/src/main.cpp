#include "Settings.h"
#include "Logger.h"
#include "EventProcessor.h"
#include "Hooks.h"

#ifndef FTA_VERSION_INCLUDED
	#define FTA_VERSION_INCLUDED

	#define MAKE_STR_HELPER(a_str) #a_str
	#define MAKE_STR(a_str) MAKE_STR_HELPER(a_str)

	#define FTA_VERSION_MAJOR 1
	#define FTA_VERSION_MINOR 0
	#define FTA_VERSION_PATCH 0
	#define FTA_VERSION_BETA 0
	#define FTA_VERSION_VERSTRING   \
		MAKE_STR(FTA_VERSION_MAJOR) \
		"." MAKE_STR(FTA_VERSION_MINOR) "." MAKE_STR(FTA_VERSION_PATCH) "." MAKE_STR(FTA_VERSION_BETA)
#endif

void SKSEMessageHandler(SKSE::MessagingInterface::Message* message)
{
	auto ui = RE::UI::GetSingleton();
	auto eventProcessor = ModernWaitMenu::EventProcessor::GetSingleton();
	auto deviceManager = RE::BSInputDeviceManager::GetSingleton();
	switch (message->type)
	{
		case (SKSE::MessagingInterface::kDataLoaded):
			if (ui)
			{
				ui->GetEventSource<RE::MenuOpenCloseEvent>()->AddEventSink(eventProcessor);
				MWM_LOG_INFO("Event Sink registered.");
			}
			break;
		case SKSE::MessagingInterface::kInputLoaded:
			SKSE::GetModCallbackEventSource()->AddEventSink(eventProcessor);

			if (deviceManager)
			{
				deviceManager->AddEventSink(eventProcessor);
				MWM_LOG_INFO("Input Event Sink registered.");
			}
			break;
		case SKSE::MessagingInterface::kPostLoadGame:
		case SKSE::MessagingInterface::kPostLoad:
		case SKSE::MessagingInterface::kNewGame:
		case SKSE::MessagingInterface::kSaveGame:
		default:
			break;
	}
}
//
SKSEPluginLoad(const SKSE::LoadInterface* skse)
{
	SKSE::Init(skse);
	ModernWaitMenu::Logger::Init();

	if (skse->IsEditor())
		return false;

	// Retrieve Settings and Initialize Hooks and Events
	ModernWaitMenu::Settings::Load();
	SKSE::GetMessagingInterface()->RegisterListener(SKSEMessageHandler);
	ModernWaitMenu::SleepWaitMenuHook::Install();

	return true;
}