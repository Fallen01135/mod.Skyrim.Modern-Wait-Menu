#include "EventProcessor.hpp"
#include "Hooks.hpp"
#include "Logger.hpp"
#include "RE/B/BSInputDeviceManager.h"
#include "RE/M/MenuOpenCloseEvent.h"
#include "RE/U/UI.h"
#include "SKSE/API.h"
#include "SKSE/Interfaces.h"
#include "spdlog/spdlog.h"
#include "Managers.hpp"


void SKSEMessageHandler(SKSE::MessagingInterface::Message *message)
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
				spdlog::info("Event Sink registered.");
			}
			break;
		case SKSE::MessagingInterface::kInputLoaded:
			spdlog::info("Input Loaded.");
			SKSE::GetModCallbackEventSource()->AddEventSink(eventProcessor);

			if (deviceManager)
			{
				deviceManager->AddEventSink(eventProcessor);
				spdlog::info("Input Event Sink registered.");
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

SKSEPluginLoad(const SKSE::LoadInterface *skse)
{
	SKSE::Init(skse);
	ModernWaitMenu::Logger::Init();
	spdlog::info("Modern Wait Menu is loading...");

	if (skse->IsEditor())
		return false;

	// Retrieve Settings and Initialize Hooks and Events
	ModernWaitMenu::SettingsManager::Load();
	spdlog::info("Settings loaded...");

	SKSE::GetMessagingInterface()->RegisterListener(SKSEMessageHandler);
	spdlog::info("SKSE Message Handler registered...");

	ModernWaitMenu::SleepWaitMenuHook::Install();
	spdlog::info("Hooks prepared...");

	spdlog::info("Modern Wait Menu loaded!");
	return true;
}