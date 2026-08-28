// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <Mib/Core/Core>
#include <Mib/CommandLine/AnsiEncodingParseInput>

using namespace NMib;
using namespace NMib::NCommandLine;
using namespace NMib::NConcurrency;
using namespace NMib::NContainer;

namespace
{
	struct CCollectingParser
	{
		CCollectingParser()
			: m_Parser
				(
					CAnsiEncodingParseInputOptions
						{
							.m_fOnKeyEvent = [this](CKeyEvent &&_KeyEvent)
							{
								m_KeyEvents.f_InsertLast(fg_Move(_KeyEvent));
							}
							, .m_fOnMouseEvent = [this](CTerminalMouseEvent &&_MouseEvent)
							{
								m_MouseEvents.f_InsertLast(fg_Move(_MouseEvent));
							}
						}
				)
		{
		}

		CAnsiEncodingParseInput m_Parser;
		TCVector<CKeyEvent> m_KeyEvents;
		TCVector<CTerminalMouseEvent> m_MouseEvents;
	};
}

struct CAnsiEncodingParseInput_Tests : public NMib::NTest::CTest
{
	void f_DoTests()
	{
		DMibTestSuite("Plain Text") -> TCFuture<void>
		{
			CCollectingParser Collector;
			Collector.m_Parser.f_AddInput("ab");

			DMibExpect(Collector.m_KeyEvents.f_GetLen(), ==, 2);
			DMibExpect(Collector.m_KeyEvents[0].m_ScanCode, ==, ch32('a'));
			DMibExpect(Collector.m_KeyEvents[0].m_Text, ==, NStr::CStr("a"));
			DMibExpect(Collector.m_KeyEvents[1].m_ScanCode, ==, ch32('b'));

			// UTF-8 multi-byte codepoint
			Collector.m_KeyEvents.f_Clear();
			Collector.m_Parser.f_AddInput("\xC3\xA5");

			DMibExpect(Collector.m_KeyEvents.f_GetLen(), ==, 1);
			DMibExpect(Collector.m_KeyEvents[0].m_ScanCode, ==, ch32(0xE5));

			co_return {};
		};

		DMibTestSuite("Control Keys") -> TCFuture<void>
		{
			CCollectingParser Collector;
			Collector.m_Parser.f_AddInput("\r\t\x7F\x11");

			DMibExpect(Collector.m_KeyEvents.f_GetLen(), ==, 4);
			DMibExpect(Collector.m_KeyEvents[0].f_Is(EKey::mc_Enter), ==, true);
			DMibExpect(Collector.m_KeyEvents[1].f_Is(EKey::mc_Tab), ==, true);
			DMibExpect(Collector.m_KeyEvents[2].f_Is(EKey::mc_Backspace), ==, true);
			DMibExpect(Collector.m_KeyEvents[3].m_ScanCode, ==, ch32('q'));
			DMibExpect(Collector.m_KeyEvents[3].m_Modifiers == EKeyModifier::mc_Ctrl, ==, true);

			co_return {};
		};

		DMibTestSuite("Escape Sequences") -> TCFuture<void>
		{
			{
				DMibTestPath("Arrows");

				CCollectingParser Collector;
				Collector.m_Parser.f_AddInput("\x1B[A\x1B[1;5C");

				DMibExpect(Collector.m_KeyEvents.f_GetLen(), ==, 2);
				DMibExpect(Collector.m_KeyEvents[0].f_Is(EKey::mc_Up, EKeyModifier::mc_None), ==, true);
				DMibExpect(Collector.m_KeyEvents[1].f_Is(EKey::mc_Right, EKeyModifier::mc_Ctrl), ==, true);
			}

			{
				DMibTestPath("Tilde Keys");

				CCollectingParser Collector;
				Collector.m_Parser.f_AddInput("\x1B[3~\x1B[5;2~");

				DMibExpect(Collector.m_KeyEvents.f_GetLen(), ==, 2);
				DMibExpect(Collector.m_KeyEvents[0].f_Is(EKey::mc_Delete, EKeyModifier::mc_None), ==, true);
				DMibExpect(Collector.m_KeyEvents[1].f_Is(EKey::mc_PageUp, EKeyModifier::mc_Shift), ==, true);
			}

			{
				DMibTestPath("Legacy Final Event Types");

				// With event type reporting the terminal appends :event to the modifier field of
				// every key form, the legacy finals included; a release must not parse as a press
				CCollectingParser Collector;
				Collector.m_Parser.f_AddInput("\x1B[1;1:3A\x1B[1;5:2C\x1B[5;2:3~\x1B[1;1:3Z");

				DMibExpect(Collector.m_KeyEvents.f_GetLen(), ==, 4);
				DMibExpect(Collector.m_KeyEvents[0].f_Is(EKey::mc_Up, EKeyModifier::mc_None), ==, true);
				DMibExpect(Collector.m_KeyEvents[0].m_EventType == EKeyEventType::mc_Release, ==, true);
				DMibExpect(Collector.m_KeyEvents[1].f_Is(EKey::mc_Right, EKeyModifier::mc_Ctrl), ==, true);
				DMibExpect(Collector.m_KeyEvents[1].m_EventType == EKeyEventType::mc_Repeat, ==, true);
				DMibExpect(Collector.m_KeyEvents[2].f_Is(EKey::mc_PageUp, EKeyModifier::mc_Shift), ==, true);
				DMibExpect(Collector.m_KeyEvents[2].m_EventType == EKeyEventType::mc_Release, ==, true);
				DMibExpect(Collector.m_KeyEvents[3].f_Is(EKey::mc_Tab, EKeyModifier::mc_Shift), ==, true);
				DMibExpect(Collector.m_KeyEvents[3].m_EventType == EKeyEventType::mc_Release, ==, true);
			}

			{
				DMibTestPath("SS3 And Shift Tab");

				CCollectingParser Collector;
				Collector.m_Parser.f_AddInput("\x1BOP\x1B[Z");

				DMibExpect(Collector.m_KeyEvents.f_GetLen(), ==, 2);
				DMibExpect(Collector.m_KeyEvents[0].f_Is(EKey::mc_F1), ==, true);
				DMibExpect(Collector.m_KeyEvents[1].f_Is(EKey::mc_Tab, EKeyModifier::mc_Shift), ==, true);
			}

			{
				DMibTestPath("Lone Escape");

				CCollectingParser Collector;
				Collector.m_Parser.f_AddInput("\x1B");

				// A trailing escape is buffered: it may be the start of a sequence split across reads
				DMibExpect(Collector.m_KeyEvents.f_GetLen(), ==, 0);
				DMibExpect(Collector.m_Parser.f_HasBufferedInput(), ==, true);

				// The caller's flush timeout resolves it as the Escape key
				Collector.m_Parser.f_Flush();

				DMibExpect(Collector.m_KeyEvents.f_GetLen(), ==, 1);
				DMibExpect(Collector.m_KeyEvents[0].f_Is(EKey::mc_Escape), ==, true);

				{
					DMibTestPath("Split Sequence");

					Collector.m_KeyEvents.f_Clear();
					Collector.m_Parser.f_AddInput("\x1B");
					Collector.m_Parser.f_AddInput("[A");

					DMibExpect(Collector.m_KeyEvents.f_GetLen(), ==, 1);
					DMibExpect(Collector.m_KeyEvents[0].f_Is(EKey::mc_Up, EKeyModifier::mc_None), ==, true);
				}

				{
					DMibTestPath("Alt Prefixed");

					Collector.m_KeyEvents.f_Clear();
					Collector.m_Parser.f_AddInput("\x1Bx");

					DMibExpect(Collector.m_KeyEvents.f_GetLen(), ==, 1);
					DMibExpect(Collector.m_KeyEvents[0].m_ScanCode, ==, ch32('x'));
					DMibExpect(Collector.m_KeyEvents[0].m_Modifiers == EKeyModifier::mc_Alt, ==, true);
				}
			}

			co_return {};
		};

		DMibTestSuite("Kitty Protocol") -> TCFuture<void>
		{
			CCollectingParser Collector;

			{
				DMibTestPath("Modifiers");

				Collector.m_Parser.f_AddInput("\x1B[113;5u");

				DMibExpect(Collector.m_KeyEvents.f_GetLen(), ==, 1);
				DMibExpect(Collector.m_KeyEvents[0].m_ScanCode, ==, ch32('q'));
				DMibExpect(Collector.m_KeyEvents[0].m_Modifiers == EKeyModifier::mc_Ctrl, ==, true);
			}

			{
				DMibTestPath("Release Event");

				Collector.m_KeyEvents.f_Clear();
				Collector.m_Parser.f_AddInput("\x1B[97;1:3u");

				DMibExpect(Collector.m_KeyEvents.f_GetLen(), ==, 1);
				DMibExpect(Collector.m_KeyEvents[0].m_ScanCode, ==, ch32('a'));
				DMibExpect(Collector.m_KeyEvents[0].m_EventType == EKeyEventType::mc_Release, ==, true);
			}

			{
				DMibTestPath("Associated Text");

				Collector.m_KeyEvents.f_Clear();
				Collector.m_Parser.f_AddInput("\x1B[97;2;65u");

				DMibExpect(Collector.m_KeyEvents.f_GetLen(), ==, 1);
				DMibExpect(Collector.m_KeyEvents[0].m_ScanCode, ==, ch32('a'));
				DMibExpect(Collector.m_KeyEvents[0].m_Modifiers == EKeyModifier::mc_Shift, ==, true);
				DMibExpect(Collector.m_KeyEvents[0].m_Text, ==, NStr::CStr("A"));
			}

			{
				DMibTestPath("Functional Key");

				Collector.m_KeyEvents.f_Clear();
				Collector.m_Parser.f_AddInput("\x1B[57364;1u");

				DMibExpect(Collector.m_KeyEvents.f_GetLen(), ==, 1);
				DMibExpect(Collector.m_KeyEvents[0].f_Is(EKey::mc_F1), ==, true);
			}

			{
				DMibTestPath("Text Synthesis");

				// Without associated text the codepoint is the text for plain strokes only:
				// a chord like ctrl+t types nothing
				Collector.m_KeyEvents.f_Clear();
				Collector.m_Parser.f_AddInput("\x1B[116;1u\x1B[116;5u");

				DMibExpect(Collector.m_KeyEvents.f_GetLen(), ==, 2);
				DMibExpect(Collector.m_KeyEvents[0].m_Text, ==, NStr::CStr("t"));
				DMibExpect(Collector.m_KeyEvents[1].m_Text, ==, NStr::CStr());
			}

			{
				DMibTestPath("Super Modifier");

				Collector.m_KeyEvents.f_Clear();
				Collector.m_Parser.f_AddInput("\x1B[122;9u");

				DMibExpect(Collector.m_KeyEvents.f_GetLen(), ==, 1);
				DMibExpect(Collector.m_KeyEvents[0].m_ScanCode, ==, ch32('z'));
				DMibExpect(Collector.m_KeyEvents[0].m_Modifiers == EKeyModifier::mc_Super, ==, true);
			}

			co_return {};
		};

		DMibTestSuite("Split Sequences") -> TCFuture<void>
		{
			CCollectingParser Collector;

			{
				DMibTestPath("Split Before Parameters");

				Collector.m_Parser.f_AddInput("\x1B[");

				DMibExpect(Collector.m_KeyEvents.f_GetLen(), ==, 0);

				Collector.m_Parser.f_AddInput("3~");

				DMibExpect(Collector.m_KeyEvents.f_GetLen(), ==, 1);
				DMibExpect(Collector.m_KeyEvents[0].f_Is(EKey::mc_Delete), ==, true);
			}

			{
				DMibTestPath("Split Inside Parameters");

				Collector.m_KeyEvents.f_Clear();
				Collector.m_Parser.f_AddInput("a\x1B[1;");

				DMibExpect(Collector.m_KeyEvents.f_GetLen(), ==, 1);
				DMibExpect(Collector.m_KeyEvents[0].m_ScanCode, ==, ch32('a'));

				Collector.m_Parser.f_AddInput("5C");

				DMibExpect(Collector.m_KeyEvents.f_GetLen(), ==, 2);
				DMibExpect(Collector.m_KeyEvents[1].f_Is(EKey::mc_Right, EKeyModifier::mc_Ctrl), ==, true);
			}

			co_return {};
		};

		DMibTestSuite("Mouse") -> TCFuture<void>
		{
			CCollectingParser Collector;

			{
				DMibTestPath("Press Release");

				Collector.m_Parser.f_AddInput("\x1B[<0;5;10M\x1B[<0;5;10m");

				DMibExpect(Collector.m_MouseEvents.f_GetLen(), ==, 2);
				DMibExpect(Collector.m_MouseEvents[0].m_EventType == EMouseEventType::mc_Down, ==, true);
				DMibExpect(Collector.m_MouseEvents[0].m_Button == EMouseButton::mc_Left, ==, true);
				DMibExpect(Collector.m_MouseEvents[0].m_CoordX, ==, 4);
				DMibExpect(Collector.m_MouseEvents[0].m_CoordY, ==, 9);
				DMibExpect(Collector.m_MouseEvents[1].m_EventType == EMouseEventType::mc_Up, ==, true);
			}

			{
				DMibTestPath("Wheel");

				Collector.m_MouseEvents.f_Clear();
				Collector.m_Parser.f_AddInput("\x1B[<64;1;1M\x1B[<65;1;1M");

				DMibExpect(Collector.m_MouseEvents.f_GetLen(), ==, 2);
				DMibExpect(Collector.m_MouseEvents[0].m_EventType == EMouseEventType::mc_Wheel, ==, true);
				DMibExpect(Collector.m_MouseEvents[0].m_WheelDelta, ==, 1.0);
				DMibExpect(Collector.m_MouseEvents[1].m_WheelDelta, ==, -1.0);
			}

			{
				DMibTestPath("Modifiers And Motion");

				Collector.m_MouseEvents.f_Clear();
				Collector.m_Parser.f_AddInput("\x1B[<16;2;2M\x1B[<32;3;3M");

				DMibExpect(Collector.m_MouseEvents.f_GetLen(), ==, 2);
				DMibExpect(Collector.m_MouseEvents[0].m_Modifiers == EKeyModifier::mc_Ctrl, ==, true);
				DMibExpect(Collector.m_MouseEvents[0].m_EventType == EMouseEventType::mc_Down, ==, true);
				DMibExpect(Collector.m_MouseEvents[1].m_EventType == EMouseEventType::mc_Move, ==, true);
			}

			co_return {};
		};
	}
};

DMibTestRegister(CAnsiEncodingParseInput_Tests, Malterlib::CommandLine);
