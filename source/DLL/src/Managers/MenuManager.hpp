// Copyright (c) 2026 Fallen01135. All rights reserved.
// Licensed under the Fallen01135 Mod License (see LICENSE.md).

#pragma once


#include "RE/G/GFxMovieView.h"
#include <array>


namespace ModernWaitMenu
{
	class MenuManager
	{
	private:
		inline static RE::GFxMovieView* _view{ nullptr };

	public:
		static void SetView(RE::GFxMovieView* view) noexcept { _view = view; };
		[[nodiscard]] static RE::GFxMovieView* GetView() noexcept { return _view; };
		[[nodiscard]] static bool IsSleepWaitMenuOpen() noexcept { return GetView() != nullptr; };
	};
}