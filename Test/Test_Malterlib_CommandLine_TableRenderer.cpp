// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <Mib/Core/Core>
#include <Mib/Test/Test>
#include <Mib/CommandLine/TableRenderer>
#include <Mib/CommandLine/AnsiEncodingParse>

namespace NMib::NCommandLine
{
	using namespace NStr;

	namespace
	{
		CStr fg_RenderTable(uint32 _Width, CStr const &_Message)
		{
			CStr Output;
			CTableRenderHelper Renderer
				(
					[&Output](CStr const &_Text)
					{
						Output += _Text;
					}
					, CTableRenderHelper::EOption_NoExtraLines
					, EAnsiEncodingFlag_BoxDrawing
					, _Width
				)
			;
			Renderer.f_AddHeadings("Repository", "Output");
			Renderer.f_AddRow("Malterlib/CommandLine", _Message);
			Renderer.f_Output();

			return Output;
		}

		umint fg_MaxLineWidth(CStr const &_Output)
		{
			umint Width = 0;
			for (auto const &Line : _Output.f_SplitLine())
				Width = fg_Max(Width, CAnsiEncodingParse::fs_RenderedStrLen(Line));

			return Width;
		}
	}

	struct CTableRenderer_Tests : NMib::NTest::CTest
	{
		void f_DoTests()
		{
			DMibTestSuite("Width")
			{
				DMibTestCategory("UnknownWidth")
				{
					CStr Message;
					Message.f_AddChars('X', 240);

					auto Output = fg_RenderTable(0, Message);

					DMibExpect(Output.f_Find(Message), >=, 0);
					DMibExpect(Output, ==, fg_RenderTable(280, Message));
					DMibExpect(fg_MaxLineWidth(Output), <=, 280);
				};

				DMibTestCategory("UnknownWidthStillWraps")
				{
					CStr Message;
					Message.f_AddChars('X', 300);

					auto Output = fg_RenderTable(0, Message);

					DMibExpect(Output.f_Find(Message), ==, -1);
					DMibExpect(Output, ==, fg_RenderTable(280, Message));
					DMibExpect(fg_MaxLineWidth(Output), <=, 280);
				};

				DMibTestCategory("ExplicitWidths")
				{
					CStr Message;
					Message.f_AddChars('X', 300);

					auto Narrow = fg_RenderTable(80, Message);
					auto Wide = fg_RenderTable(340, Message);

					DMibExpect(Narrow.f_Find(Message), ==, -1);
					DMibExpect(fg_MaxLineWidth(Narrow), <=, 80);
					DMibExpect(Wide.f_Find(Message), >=, 0);
					DMibExpect(fg_MaxLineWidth(Wide), >, 280);
					DMibExpect(fg_MaxLineWidth(Wide), <=, 340);
				};
			};
		}
	};

	DMibTestRegister(CTableRenderer_Tests, Malterlib::CommandLine);
}
