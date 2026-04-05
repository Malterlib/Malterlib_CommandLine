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
			DMibTestSuite("LineBreak")
			{
				CAnsiEncoding AnsiEncoding(EAnsiEncodingFlag_Color | EAnsiEncodingFlag_BoxDrawing | EAnsiEncodingFlag_Color24Bit);

				DMibExpect(AnsiEncoding.f_LineBreak("\n\nTest\n", 10), ==, (NContainer::TCVector<CAnsiEncoding::CLine>{{"", 0}, {"", 0}, {"Test", 4}, {"", 0}}));
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
