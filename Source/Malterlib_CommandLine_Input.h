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
		, mc_Keypad0 = 57399
		, mc_Keypad1 = 57400
		, mc_Keypad2 = 57401
		, mc_Keypad3 = 57402
		, mc_Keypad4 = 57403
		, mc_Keypad5 = 57404
		, mc_Keypad6 = 57405
		, mc_Keypad7 = 57406
		, mc_Keypad8 = 57407
		, mc_Keypad9 = 57408
		, mc_KeypadDecimal = 57409
		, mc_KeypadDivide = 57410
		, mc_KeypadMultiply = 57411
		, mc_KeypadSubtract = 57412
		, mc_KeypadAdd = 57413
		, mc_KeypadEnter = 57414
		, mc_KeypadEqual = 57415
		, mc_KeypadSeparator = 57416
		, mc_KeypadLeft = 57417
		, mc_KeypadRight = 57418
		, mc_KeypadUp = 57419
		, mc_KeypadDown = 57420
		, mc_KeypadPageUp = 57421
		, mc_KeypadPageDown = 57422
		, mc_KeypadHome = 57423
		, mc_KeypadEnd = 57424
		, mc_KeypadInsert = 57425
		, mc_KeypadDelete = 57426
		, mc_KeypadBegin = 57427
		, mc_LeftShift = 57441
		, mc_LeftControl = 57442
		, mc_LeftAlt = 57443
		, mc_LeftSuper = 57444
		, mc_LeftHyper = 57445
		, mc_LeftMeta = 57446
		, mc_RightShift = 57447
		, mc_RightControl = 57448
		, mc_RightAlt = 57449
		, mc_RightSuper = 57450
		, mc_RightHyper = 57451
		, mc_RightMeta = 57452
	};

	struct CKeyEvent
	{
		NStr::CStr m_Text;
		ch32 m_ScanCode = 0;
		ch32 m_BaseLayoutKey = 0; // What the key at the same position is in the US layout. The same as m_ScanCode where the source of the event does not say
		EKeyModifier m_Modifiers = EKeyModifier::mc_None;
		EKeyEventType m_EventType = EKeyEventType::mc_Press;
		uint16 m_HandlingReportID = 0; // Nonzero while the terminal awaits a handling report for a deferred keybinding.

		bool f_Is(EKey _Key) const;
		bool f_Is(EKey _Key, EKeyModifier _Modifiers) const;
		bool f_IsBaseLayout(EKey _Key) const;
		bool f_IsBaseLayout(ch32 _Key) const;

		template <typename tf_CStream>
		void f_Stream(tf_CStream &_Stream);

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
