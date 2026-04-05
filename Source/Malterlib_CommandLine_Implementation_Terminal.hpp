// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <Mib/Encoding/JsonShortcuts>

namespace NMib::NCommandLine
{
	using namespace NStr;
	using namespace NFile;
	using namespace NStorage;
	using namespace NEncoding;
	using namespace NContainer;
	using namespace NCommandLine;

	template <typename t_CCustomization>
	void TCCommandLineSpecification<t_CCustomization>::f_AddTerminalOptions()
	{
		f_RegisterGlobalOptions
			(
				{
					"Color?"_o=
					{
						"Names"_o= _o["--color"]
						, "Default"_o= CCommandLineDefaults::fs_ColorEnabledDefault()
						, "Description"_o= "Display text output with ansi colors where supported.\n"\
						"You can override behaviour with MalterlibColor env var. Set it to true or false. "
					}
				}
			)
		;
		f_RegisterGlobalOptions
			(
				{
					"Color24Bit?"_o=
					{
						"Names"_o= _o["--color-24bit"]
						, "Default"_o= CCommandLineDefaults::fs_Color24BitEnabledDefault()
						, "Description"_o= "Display text output with 24 bit ansi colors.\n"\
						"By default detected through COLORTERM. "\
						"You can override behaviour with MalterlibColor24Bit env var. Set it to true or false. "
					}
				}
			)
		;
		f_RegisterGlobalOptions
			(
				{
					"ColorLight?"_o=
					{
						"Names"_o= _o["--color-light"]
						, "Default"_o= CCommandLineDefaults::fs_ColorLightBackgroundDefault()
						, "Description"_o= "Force light background.\n"\
						"You can override behaviour with MalterlibColorLight env var. Set it to true, false. "
					}
				}
			)
		;
		f_RegisterGlobalOptions
			(
				{
					"BoxDrawing?"_o=
					{
						"Names"_o= _o["--box-drawing"]
						, "Default"_o= CCommandLineDefaults::fs_BoxDrawingDefault()
						, "Description"_o= "Enable box drawing characters.\n"\
					}
				}
			)
		;
		f_RegisterGlobalOptions
			(
				{
					"TerminalWidth?"_o=
					{
						"Names"_o= _o["--terminal-width"]
						, "Default"_o= fg_GetSys()->f_GetEnvironmentVariable("MalterlibTerminalWidth", "-1").f_ToInt(int64(-1))
						, "Description"_o= "Override detected terminal width.\n"\
						"Useful for when for whatever reason tty is not accessible. "\
					}
				}
			)
		;
		f_RegisterGlobalOptions
			(
				{
					"TerminalHeight?"_o=
					{
						"Names"_o= _o["--terminal-height"]
						, "Default"_o= fg_GetSys()->f_GetEnvironmentVariable("MalterlibTerminalHeight", "-1").f_ToInt(int64(-1))
						, "Description"_o= "Override detected terminal height.\n"\
						"Useful for when for whatever reason tty is not accessible. "\
					}
				}
			)
		;
	}
}
