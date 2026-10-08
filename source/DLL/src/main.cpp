// Copyright (c) 2026 Fallen01135. All rights reserved.
// Licensed under the Fallen01135 Mod License (see LICENSE.md).

#include "Core/Logger.hpp"
#include "Events/InputEventSink.hpp"
#include "Events/MenuEventSink.hpp"
#include "Events/ModEventSink.hpp"
#include "Hooks/Hooks.hpp"
#include "Managers/SettingsManager.hpp"
#include "RE/B/BSInputDeviceManager.h"
#include "RE/M/MenuOpenCloseEvent.h"
#include "RE/U/UI.h"
#include "SKSE/API.h"
#include "SKSE/Interfaces.h"
#include "spdlog/spdlog.h"
#include <fmt/format.h>

/**
 * ATTENTION!
 * 
 * The Variable "SKYRIMVR" defines SE 1.5.97 AND VR versions
 * Therefore, everything marked in all cpp and hpp files with SKYRIMVR
 * is ALWAYS SE 1.5.97 AND VR
 */
#ifndef SKYRIMVR
// For Skyrim SE / AE
extern "C" DLLEXPORT constinit auto SKSEPlugin_Version = []()
{
	SKSE::PluginVersionData v{};

	v.PluginVersion(Plugin::VERSION);
	v.PluginName(Plugin::NAME);
	v.AuthorName("Fallen011[35]");

	v.UsesAddressLibrary(true);
	v.HasNoStructUse(false);
	v.UsesStructsPost629(true);

	return v;
}();
#endif

// For Skyrim VR
extern "C" DLLEXPORT bool SKSEAPI SKSEPlugin_Query(const SKSE::QueryInterface* a_skse, SKSE::PluginInfo* a_info)
{
	a_info->infoVersion = SKSE::PluginInfo::kVersion;
	a_info->name = Plugin::NAME.data();
	a_info->version = Plugin::VERSION[0];

	if (a_skse->IsEditor())
	{
		spdlog::critical("Loaded in editor, marking as incompatible");
		return false;
	}

	return true;
}

void SKSEMessageHandler(SKSE::MessagingInterface::Message *message)
{
	switch (message->type)
	{
		case (SKSE::MessagingInterface::kDataLoaded):
			if (auto ui = RE::UI::GetSingleton())
			{
				ui->GetEventSource<RE::MenuOpenCloseEvent>()->AddEventSink(ModernWaitMenu::MenuEventSink::GetSingleton());
				spdlog::info("Event Sink registered.");
			}
			break;
		case SKSE::MessagingInterface::kInputLoaded:
			spdlog::info("Input Loaded.");
			SKSE::GetModCallbackEventSource()->AddEventSink(ModernWaitMenu::ModEventSink::GetSingleton());

			if (auto deviceManager = RE::BSInputDeviceManager::GetSingleton())
			{
				deviceManager->AddEventSink(ModernWaitMenu::InputEventSink::GetSingleton());
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

extern "C" DLLEXPORT bool SKSEAPI SKSEPlugin_Load(const SKSE::LoadInterface* skse)
{
	SKSE::Init(skse);

	ModernWaitMenu::Logger::Init();
	spdlog::info("Modern Wait Menu is loading...");
	spdlog::info("{} v{}", Plugin::NAME.data(), Plugin::VERSION.string());

	// Retrieve Settings and Initialize Hooks and Events
	ModernWaitMenu::SettingsManager::Load();
	spdlog::info("Settings loaded...");

	auto messaging = SKSE::GetMessagingInterface();
	if (messaging)
	{
		messaging->RegisterListener(SKSEMessageHandler);
		spdlog::info("SKSE Message Handler registered...");
	}

	ModernWaitMenu::Hooks::Install();
	spdlog::info("Hooks prepared...");

	spdlog::info("Modern Wait Menu loaded!");
	return true;
}