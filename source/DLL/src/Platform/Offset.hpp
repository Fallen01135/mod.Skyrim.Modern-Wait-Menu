// Copyright (c) 2026 Fallen01135. All rights reserved.
// Licensed under the Fallen01135 Mod License (see LICENSE.md).

#pragma once


#include "REL/Relocation.h"


/**
 * ATTENTION!
 *
 * The Variable "SKYRIMVR" defines SE 1.5.97 AND VR versions
 * Therefore, everything marked in all cpp and hpp files with SKYRIMVR
 * is ALWAYS SE 1.5.97 AND VR
 */
namespace Offset
{
#ifndef SKYRIMVR
	namespace SleepWaitMenu
	{
		// SE = 269872 , SE2 = 215936
		constexpr REL::ID Vtbl(215936);
	}
#else
	namespace SleepWaitMenu
	{
		constexpr REL::Offset Vtbl(0x173fc88);
	}
#endif

	namespace Time
	{
#ifndef SKYRIMVR
		// SE = 523660 , SE2 = 410199
		constexpr REL::ID FrameTimer(410199);
#else
		constexpr REL::Offset FrameTimer(0x30c3a08);
#endif
	}
}