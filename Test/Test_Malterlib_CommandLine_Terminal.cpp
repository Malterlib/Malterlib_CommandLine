// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <Mib/Core/Core>
#include <Mib/Test/Test>

#include <Mib/CommandLine/AnsiEncoding>
#include <Mib/CommandLine/AnsiEncodingParse>
#include <Mib/CommandLine/Terminal>
#include <Mib/Process/Platform>
#include <Mib/Process/StdIn>
#include <Mib/Cryptography/RandomID>
#include <deque>

namespace NMib::NCommandLine
{
	using namespace NStr;

	class CTerminal_Tests : public NMib::NTest::CTest
	{
	public:
		void f_DoTests()
		{
			DMibTestSuite("TextCJK")
			{
				CAnsiEncoding AnsiEncoding(EAnsiEncodingFlag_Color | EAnsiEncodingFlag_BoxDrawing | EAnsiEncodingFlag_Color24Bit);
				CTerminalSurface Surface(AnsiEncoding, {20, 4});

				// CJK codepoints occupy one cell because double-width rendering is unsupported.
				CTextOptions Options;
				Options.m_DestinationArea = CTerminalArea{{0, 0}, {20, 1}};
				CTerminalArea TextArea = Surface.f_Text("日本語", Options);

				DMibExpect(TextArea.m_Size.m_Width, ==, 3);
				DMibExpect(Surface.f_GetChixel({0, 0}).f_GetCharacter(), ==, ch32(U'日'));
				DMibExpect(Surface.f_GetChixel({1, 0}).f_GetCharacter(), ==, ch32(U'本'));
				DMibExpect(Surface.f_GetChixel({2, 0}).f_GetCharacter(), ==, ch32(U'語'));
				DMibExpect(Surface.f_GetChixel({3, 0}).f_GetCharacter(), ==, ch32(' '));

				Options.m_DestinationArea = CTerminalArea{{0, 1}, {20, 1}};
				TextArea = Surface.f_Text("中文字符", Options);

				DMibExpect(TextArea.m_Size.m_Width, ==, 4);
				DMibExpect(Surface.f_GetChixel({0, 1}).f_GetCharacter(), ==, ch32(U'中'));
				DMibExpect(Surface.f_GetChixel({3, 1}).f_GetCharacter(), ==, ch32(U'符'));

				CStr Rendered = Surface.f_Render(ETerminalRenderFlag::mc_None);
				DMibExpect(Rendered.f_Find("日本語") >= 0, ==, true);
				DMibExpect(Rendered.f_Find("中文字符") >= 0, ==, true);
			};

			DMibTestSuite("TextOverflow")
			{
				CAnsiEncoding AnsiEncoding(EAnsiEncodingFlag_Color | EAnsiEncodingFlag_BoxDrawing | EAnsiEncodingFlag_Color24Bit);
				CTerminalSurface Surface(AnsiEncoding, {10, 3});

				// U+0301 is a combining acute accent.
				CTextOptions Options;
				Options.m_DestinationArea = CTerminalArea{{0, 0}, {10, 1}};
				CTerminalArea TextArea = Surface.f_Text("xe\u0301y", Options);

				DMibExpect(TextArea.m_Size.m_Width, ==, 3);
				DMibExpect(Surface.f_GetChixel({0, 0}).f_GetCharacter(), ==, ch32('x'));
				DMibExpect(Surface.f_GetChixel({1, 0}).f_GetCharacter(), ==, ch32('e'));
				DMibExpect(Surface.f_GetChixel({2, 0}).f_GetCharacter(), ==, ch32('y'));

				DMibExpect(Surface.f_GetChixel({0, 0}).m_bHasOverflow, ==, false);
				DMibExpect(Surface.f_GetChixel({1, 0}).m_bHasOverflow, ==, true);
				DMibExpect(Surface.f_GetOverflow({0, 0}) == nullptr, ==, true);

				NStr::CStr const *pOverflow = Surface.f_GetOverflow({1, 0});
				DMibExpect(pOverflow != nullptr, ==, true);
				DMibExpect(pOverflow ? *pOverflow : CStr(), ==, CStr("\u0301"));

				Options.m_DestinationArea = CTerminalArea{{0, 1}, {10, 1}};
				Surface.f_Text("語\u0301", Options);

				DMibExpect(Surface.f_GetChixel({0, 1}).f_GetCharacter(), ==, ch32(U'語'));

				NStr::CStr const *pCJKOverflow = Surface.f_GetOverflow({0, 1});
				DMibExpect(pCJKOverflow != nullptr, ==, true);
				DMibExpect(pCJKOverflow ? *pCJKOverflow : CStr(), ==, CStr("\u0301"));

				CStr Rendered = Surface.f_Render(ETerminalRenderFlag::mc_None);
				DMibExpect(Rendered.f_Find("e\u0301") >= 0, ==, true);
				DMibExpect(Rendered.f_Find("語\u0301") >= 0, ==, true);

				CTerminalSurface Destination(AnsiEncoding, {10, 3});
				Destination.f_Blit(Surface, Surface.f_Area(), {0, 0});

				DMibExpect(Destination.f_GetChixel({1, 0}).f_GetCharacter(), ==, ch32('e'));

				NStr::CStr const *pBlitOverflow = Destination.f_GetOverflow({1, 0});
				DMibExpect(pBlitOverflow != nullptr, ==, true);
				DMibExpect(pBlitOverflow ? *pBlitOverflow : CStr(), ==, CStr("\u0301"));

				CTerminalSurface Merged(AnsiEncoding, {10, 3});
				Merged.f_BlitMerge(Surface, Surface.f_Area(), {0, 0});

				NStr::CStr const *pMergeOverflow = Merged.f_GetOverflow({1, 0});
				DMibExpect(pMergeOverflow != nullptr, ==, true);
				DMibExpect(pMergeOverflow ? *pMergeOverflow : CStr(), ==, CStr("\u0301"));

				// The two-cell width keeps the base and combining codepoint within the same line.
				CTerminalSurface Changed(Surface);
				Options.m_DestinationArea = CTerminalArea{{1, 0}, {2, 1}};
				Changed.f_Text("e\u0300", Options); // U+0300 combining grave

				DMibExpect(Changed.f_GetChixel({1, 0}) == Surface.f_GetChixel({1, 0}), ==, true);
				CStr Diff = Changed.f_DiffRender(Surface, ETerminalRenderFlag::mc_None);
				DMibExpect(Diff.f_Find("e\u0300") >= 0, ==, true);

				CTerminalSurface Same(Surface);
				CStr NoDiff = Same.f_DiffRender(Surface, ETerminalRenderFlag::mc_None);
				DMibExpect(NoDiff.f_Find("e\u0301") >= 0, ==, false);

				Options.m_DestinationArea = CTerminalArea{{1, 0}, {1, 1}};
				Changed.f_Text("z", Options);

				DMibExpect(Changed.f_GetChixel({1, 0}).f_GetCharacter(), ==, ch32('z'));
				DMibExpect(Changed.f_GetChixel({1, 0}).m_bHasOverflow, ==, false);
				DMibExpect(Changed.f_GetOverflow({1, 0}) == nullptr, ==, true);

				Options.m_DestinationArea = CTerminalArea{{1, 0}, {2, 1}};
				Changed.f_Text("a\u0308", Options); // U+0308 combining diaeresis

				DMibExpect(Changed.f_GetChixel({1, 0}).f_GetCharacter(), ==, ch32('a'));

				NStr::CStr const *pFreshOverflow = Changed.f_GetOverflow({1, 0});
				DMibExpect(pFreshOverflow != nullptr, ==, true);
				DMibExpect(pFreshOverflow ? *pFreshOverflow : CStr(), ==, CStr("\u0308"));

				Changed.f_Clear();
				DMibExpect(Changed.f_GetChixel({1, 0}).m_bHasOverflow == false, ==, true);
				DMibExpect(Changed.f_GetOverflow({1, 0}) != nullptr, ==, false);
			};
		}
	};

	DMibTestRegister(CTerminal_Tests, Malterlib::CommandLine);
}
