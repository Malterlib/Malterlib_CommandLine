// Copyright © 2015 Hansoft AB 
// Distributed under the MIT license, see license text in LICENSE.Malterlib

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

				DMibExpect(AnsiEncoding.f_LineBreak("\n\nTest\n", 10), ==, (NContainer::TCVector<CStr>{"", "", "Test", ""}));
				DMibExpect
					(
						AnsiEncoding.f_LineBreak
						(
							"\n\nTest Testing TestTestTestTest 22\n"
							, 10
							, CAnsiEncoding::EWordWrap_Word)
							, ==
							, (NContainer::TCVector<CStr>{"", "", "Test", "Testing", "TestTestTe", "stTest 22", ""})
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
							, (NContainer::TCVector<CStr>{"", "", "Test Testi", "ng TestTes", "tTestTest", "22", ""})
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
							, (NContainer::TCVector<CStr>{"Test Test…"})
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
							, (NContainer::TCVector<CStr>{"Test Testi"})
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
							, (NContainer::TCVector<CStr>{"", "", "Test", "Testing", "TestTestT…", "…estTest", "22", ""})
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
							, (NContainer::TCVector<CStr>{"Test", "{", "    TestV…", "…alue", "\"Value\"", "}", ""})
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
							, (NContainer::TCVector<CStr>{"Test", "{", "    TestV…", "…alue \"Va…", "…lue\"", "}", ""})
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
							, (NContainer::TCVector<CStr>{"Test"})
					)
				;
			};
		}
	};
	DMibTestRegister(CAnsiEncoding_Tests, Malterlib::CommandLine);
}
