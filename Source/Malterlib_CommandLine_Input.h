// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

namespace NMib::NCommandLine
{
	enum class EKeyModifier : uint8
	{
		mc_None = 0
		, mc_Shift = fg_Bit(0)
		, mc_Alt = fg_Bit(1)
		, mc_Ctrl = fg_Bit(2)
		, mc_Super = fg_Bit(3)
		, mc_Hyper = fg_Bit(4)
		, mc_Meta = fg_Bit(5)
		, mc_CapsLock = fg_Bit(6)
		, mc_NumLock = fg_Bit(7)
	};

	enum class EKeyEventType : uint8
	{
		mc_Press
		, mc_Repeat
		, mc_Release
	};

	// Canonical key codes. Printable keys use their Unicode codepoint; functional keys use the kitty
	// keyboard protocol assignments (https://sw.kovidgoyal.net/kitty/keyboard-protocol/) so CSI u
	// events map one to one and legacy escape sequences translate to the same codes.
	enum class EKey : ch32
	{
		mc_None = 0
		, mc_Tab = 9
		, mc_Enter = 13
		, mc_Escape = 27
		, mc_Space = 32
		, mc_Backspace = 127
		, mc_Insert = 57348
		, mc_Delete = 57349
		, mc_Left = 57350
		, mc_Right = 57351
		, mc_Up = 57352
		, mc_Down = 57353
		, mc_PageUp = 57354
		, mc_PageDown = 57355
		, mc_Home = 57356
		, mc_End = 57357
		, mc_CapsLock = 57358
		, mc_ScrollLock = 57359
		, mc_NumLock = 57360
		, mc_PrintScreen = 57361
		, mc_Pause = 57362
		, mc_Menu = 57363
		, mc_F1 = 57364
		, mc_F2 = 57365
		, mc_F3 = 57366
		, mc_F4 = 57367
		, mc_F5 = 57368
		, mc_F6 = 57369
		, mc_F7 = 57370
		, mc_F8 = 57371
		, mc_F9 = 57372
		, mc_F10 = 57373
		, mc_F11 = 57374
		, mc_F12 = 57375
	};

	struct CKeyEvent
	{
		NStr::CStr m_Text;
		ch32 m_ScanCode = 0;
		EKeyModifier m_Modifiers = EKeyModifier::mc_None;
		EKeyEventType m_EventType = EKeyEventType::mc_Press;

		bool f_Is(EKey _Key) const;
		bool f_Is(EKey _Key, EKeyModifier _Modifiers) const;

		static NStr::CStr fs_ModifiersToString(EKeyModifier _Modifiers);
	};

	enum class EMouseButton : uint8
	{
		mc_None = 0
		, mc_Left
		, mc_Middle
		, mc_Right
	};

	enum class EMouseEventType : uint8
	{
		mc_Down
		, mc_Up
		, mc_Move
		, mc_Wheel
	};

	// Positions are terminal cell coordinates (0-based); consumers working in another coordinate
	// space convert before dispatching further
	struct CTerminalMouseEvent
	{
		int32 m_CoordX = 0;
		int32 m_CoordY = 0;
		EMouseButton m_Button = EMouseButton::mc_None;
		EMouseEventType m_EventType = EMouseEventType::mc_Down;
		EKeyModifier m_Modifiers = EKeyModifier::mc_None;
		fp64 m_WheelDelta = 0.0; // Positive scrolls up
		fp64 m_WheelDeltaX = 0.0; // Positive scrolls left
	};
}

#ifndef DMibPNoShortCuts
	using namespace NMib::NCommandLine;
#endif

#include "Malterlib_CommandLine_Input.hpp"
