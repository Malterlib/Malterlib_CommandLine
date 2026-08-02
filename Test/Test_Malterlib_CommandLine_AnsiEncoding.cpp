// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <Mib/Core/Core>
#include <Mib/Test/Test>

#include <Mib/CommandLine/AnsiEncoding>
#include <Mib/CommandLine/AnsiEncodingParse>

namespace NMib::NCommandLine
{
	using namespace NStr;

	class CAnsiEncoding_Tests : public NMib::NTest::CTest
	{
	public:
		void f_DoTests()
		{
			DMibTestSuite("LineBreakParsed")
			{
				CAnsiEncodingParse::CParsedText Parsed;

				{
					DMibTestPath("Plain");

					// Plain text: one line, one run, no properties; the run views the source string
					CAnsiEncodingParse::fs_LineBreak("Hello", 10, CAnsiEncoding::EWordWrap_Word, Parsed);

					DMibExpect(Parsed.m_Lines.f_GetLen(), ==, 1);
					DMibExpect(Parsed.m_Lines[0].m_Width, ==, 5);
					DMibExpect(Parsed.m_Lines[0].m_nRuns, ==, 1);
					DMibExpect(Parsed.m_Runs[0].m_Text, ==, CStr("Hello"));
					DMibExpect(Parsed.m_Runs[0].m_Properties == CAnsiEncodingParse::CActiveProperties{}, ==, true);
					DMibExpect(Parsed.m_Source, ==, CStr("Hello"));
					DMibExpect(Parsed.m_Runs[0].m_Text.f_GetStr() == Parsed.m_Source.f_GetStr(), ==, true);
				}

				{
					DMibTestPath("Wrapped");

					// Word wrap produces one run per line for plain text
					CAnsiEncodingParse::fs_LineBreak("Test Testing", 7, CAnsiEncoding::EWordWrap_Word, Parsed);

					DMibExpect(Parsed.m_Lines.f_GetLen(), ==, 2);
					DMibExpect(Parsed.m_Runs[Parsed.m_Lines[0].m_iFirstRun].m_Text, ==, CStr("Test"));
					DMibExpect(Parsed.m_Lines[0].m_Width, ==, 4);
					DMibExpect(Parsed.m_Runs[Parsed.m_Lines[1].m_iFirstRun].m_Text, ==, CStr("Testing"));
					DMibExpect(Parsed.m_Lines[1].m_Width, ==, 7);
				}

				{
					DMibTestPath("Styled");

					// Styled text becomes separate runs carrying the active properties, with the
					// escape sequences stripped from the run text
					CAnsiEncodingParse::fs_LineBreak("A\x1B[1m\x1B[38;2;255;0;0mRed\x1B[0mB", 20, CAnsiEncoding::EWordWrap_Word, Parsed);

					DMibExpect(Parsed.m_Lines.f_GetLen(), ==, 1);
					DMibExpect(Parsed.m_Lines[0].m_Width, ==, 5);
					DMibExpect(Parsed.m_Lines[0].m_nRuns, ==, 3);
					DMibExpect(Parsed.m_Runs[0].m_Text, ==, CStr("A"));
					DMibExpect(Parsed.m_Runs[0].m_Properties == CAnsiEncodingParse::CActiveProperties{}, ==, true);
					DMibExpect(Parsed.m_Runs[1].m_Text, ==, CStr("Red"));
					DMibExpect(Parsed.m_Runs[1].m_Properties.m_Weight ? true : false, ==, true);
					DMibExpect(Parsed.m_Runs[1].m_Properties.m_Weight && Parsed.m_Runs[1].m_Properties.m_Weight->m_Weight == CAnsiEncoding::EWeight::mc_Bold, ==, true);
					DMibExpect(Parsed.m_Runs[1].m_Properties.m_ForegroundColor ? true : false, ==, true);
					DMibExpect(Parsed.m_Runs[1].m_Properties.m_ForegroundColor && Parsed.m_Runs[1].m_Properties.m_ForegroundColor->m_Red == 255, ==, true);
					DMibExpect(Parsed.m_Runs[2].m_Text, ==, CStr("B"));
					DMibExpect(Parsed.m_Runs[2].m_Properties == CAnsiEncodingParse::CActiveProperties{}, ==, true);

					// The source is the stripped text and the styled runs view into it
					DMibExpect(Parsed.m_Source, ==, CStr("ARedB"));
					DMibExpect(Parsed.m_Runs[1].m_Text.f_GetStr() == Parsed.m_Source.f_GetStr() + 1, ==, true);
				}

				{
					DMibTestPath("Ellipsis");

					// The trailing ellipsis is a run of its own viewing constant storage
					CAnsiEncodingParse::fs_LineBreak("Test Testing TestTestTestTest 22", 10, CAnsiEncoding::EWordWrap_Ellipsis, Parsed);

					DMibExpect(Parsed.m_Lines.f_GetLen(), ==, 1);
					DMibExpect(Parsed.m_Lines[0].m_Width, ==, 10);
					DMibExpect(Parsed.m_Lines[0].m_nRuns, ==, 2);
					DMibExpect(Parsed.m_Runs[0].m_Text, ==, CStr("Test Test"));
					DMibExpect(Parsed.m_Runs[1].m_Text, ==, CStr("…"));
				}

				{
					DMibTestPath("WordEllipsis");

					// Continuation lines lead with an ellipsis run before the source view
					CAnsiEncodingParse::fs_LineBreak("TestTestTestTest", 10, CAnsiEncoding::EWordWrap_WordEllipsis, Parsed);

					DMibExpect(Parsed.m_Lines.f_GetLen(), ==, 2);
					DMibExpect(Parsed.m_Lines[0].m_Width, ==, 10);
					DMibExpect(Parsed.m_Lines[0].m_nRuns, ==, 2);
					DMibExpect(Parsed.m_Runs[Parsed.m_Lines[0].m_iFirstRun].m_Text, ==, CStr("TestTestT"));
					DMibExpect(Parsed.m_Runs[Parsed.m_Lines[0].m_iFirstRun + 1].m_Text, ==, CStr("…"));
					DMibExpect(Parsed.m_Lines[1].m_Width, ==, 8);
					DMibExpect(Parsed.m_Lines[1].m_nRuns, ==, 2);
					DMibExpect(Parsed.m_Runs[Parsed.m_Lines[1].m_iFirstRun].m_Text, ==, CStr("…"));
					DMibExpect(Parsed.m_Runs[Parsed.m_Lines[1].m_iFirstRun + 1].m_Text, ==, CStr("estTest"));
				}

				{
					DMibTestPath("CJK");

					// CJK text keeps one cell per codepoint in the parsed widths as well
					CAnsiEncodingParse::fs_LineBreak("日本語のテキスト", 4, CAnsiEncoding::EWordWrap_Character, Parsed);

					DMibExpect(Parsed.m_Lines.f_GetLen(), ==, 2);
					DMibExpect(Parsed.m_Runs[Parsed.m_Lines[0].m_iFirstRun].m_Text, ==, CStr("日本語の"));
					DMibExpect(Parsed.m_Lines[0].m_Width, ==, 4);
					DMibExpect(Parsed.m_Runs[Parsed.m_Lines[1].m_iFirstRun].m_Text, ==, CStr("テキスト"));
					DMibExpect(Parsed.m_Lines[1].m_Width, ==, 4);
				}
			};

			DMibTestSuite("LineBreak")
			{
				CAnsiEncoding AnsiEncoding(EAnsiEncodingFlag_Color | EAnsiEncodingFlag_BoxDrawing | EAnsiEncodingFlag_Color24Bit);

				DMibExpect(AnsiEncoding.f_LineBreak("\n\nTest\n", 10), ==, (NContainer::TCVector<CAnsiEncoding::CLine>{{"", 0}, {"", 0}, {"Test", 4}, {"", 0}}));

				// Multi byte UTF-8: every codepoint is one cell, so CJK text breaks per codepoint
				DMibExpect
					(
						AnsiEncoding.f_LineBreak("中文字符", 10)
						, ==
						, (NContainer::TCVector<CAnsiEncoding::CLine>{{"中文字符", 4}})
					)
				;
				DMibExpect
					(
						AnsiEncoding.f_LineBreak("日本語のテキスト", 4, CAnsiEncoding::EWordWrap_Character)
						, ==
						, (NContainer::TCVector<CAnsiEncoding::CLine>{{"日本語の", 4}, {"テキスト", 4}})
					)
				;

				// Combining characters attach to the preceding cell and do not add to the width
				DMibExpect
					(
						AnsiEncoding.f_LineBreak("xe\u0301y", 10)
						, ==
						, (NContainer::TCVector<CAnsiEncoding::CLine>{{"xe\u0301y", 3}})
					)
				;
				DMibExpect
					(
						AnsiEncoding.f_LineBreak
						(
							"\n\nTest Testing TestTestTestTest 22\n"
							, 10
							, CAnsiEncoding::EWordWrap_Word)
							, ==
							, (NContainer::TCVector<CAnsiEncoding::CLine>{{"", 0}, {"", 0}, {"Test", 4}, {"Testing", 7}, {"TestTestTe", 10}, {"stTest 22", 9}, {"", 0}}
						)
					)
				;
				DMibExpect
					(
						AnsiEncoding.f_LineBreak
						(
							"\n\nTest Testing TestTestTestTest 22\n"
							, 10
							, CAnsiEncoding::EWordWrap_Character)
							, ==
							, (NContainer::TCVector<CAnsiEncoding::CLine>{{"", 0}, {"", 0}, {"Test Testi", 10}, {"ng TestTes", 10}, {"tTestTest", 9}, {"22", 2}, {"", 0}}
						)
					)
				;
				DMibExpect
					(
						AnsiEncoding.f_LineBreak
						(
							"Test Testing TestTestTestTest 22"
							, 10
							, CAnsiEncoding::EWordWrap_Ellipsis)
							, ==
							, (NContainer::TCVector<CAnsiEncoding::CLine>{{"Test Test…", 10}}
						)
					)
				;
				DMibExpect
					(
						AnsiEncoding.f_LineBreak
						(
							"Test Testing TestTestTestTest 22"
							, 10
							, CAnsiEncoding::EWordWrap_None)
							, ==
							, (NContainer::TCVector<CAnsiEncoding::CLine>{{"Test Testi", 10}}
						)
					)
				;
				DMibExpect
					(
						AnsiEncoding.f_LineBreak
						(
							"\n\nTest Testing TestTestTestTest 22\n"
							, 10
							, CAnsiEncoding::EWordWrap_WordEllipsis)
							, ==
							, (NContainer::TCVector<CAnsiEncoding::CLine>{{"", 0}, {"", 0}, {"Test", 4}, {"Testing", 7}, {"TestTestT…", 10}, {"…estTest", 8}, {"22", 2}, {"", 0}}
						)
					)
				;
				DMibExpect
					(
						AnsiEncoding.f_LineBreak
						(
							"Test\n{\n    TestValue \"Value\"\n}\n"
							, 10
							, CAnsiEncoding::EWordWrap_WordEllipsis)
							, ==
							, (NContainer::TCVector<CAnsiEncoding::CLine>{{"Test", 4}, {"{", 1}, {"    TestV…", 10}, {"…alue", 5}, {"\"Value\"", 7}, {"}", 1}, {"", 0}}
						)
					)
				;
				DMibExpect
					(
						AnsiEncoding.f_LineBreak
						(
							"Test\n{\n    TestValue \"Value\"\n}\n"
							, 10
							, CAnsiEncoding::EWordWrap_CharacterEllipsis)
							, ==
							, (NContainer::TCVector<CAnsiEncoding::CLine>{{"Test", 4}, {"{", 1}, {"    TestV…", 10}, {"…alue \"Va…", 10}, {"…lue\"", 5}, {"}", 1}, {"", 0}}
						)
					)
				;
				DMibExpect
					(
						AnsiEncoding.f_LineBreak
						(
							"Test\n{\n    TestValue \"Value\"\n}\n"
							, 10
							, CAnsiEncoding::EWordWrap_Ellipsis)
							, ==
							, (NContainer::TCVector<CAnsiEncoding::CLine>{{"Test", 4}, {"{", 1}, {"    TestV…", 10}, {"}", 1}, {"", 0}}
						)
					)
				;
				DMibExpect
					(
						AnsiEncoding.f_LineBreak
						(
							"Test\n{\n    TestValue \"Value\"\n}\n"
							, 10
							, CAnsiEncoding::EWordWrap_None)
							, ==
							, (NContainer::TCVector<CAnsiEncoding::CLine>{{"Test", 4}, {"{", 1}, {"    TestVa", 10}, {"}", 1}, {"", 0}}
						)
					)
				;
			};
		}
	};
	DMibTestRegister(CAnsiEncoding_Tests, Malterlib::CommandLine);
}
