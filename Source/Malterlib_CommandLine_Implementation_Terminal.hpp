// Copyright © 2015 Hansoft AB 
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#include <Mib/Encoding/JSONShortcuts>

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
					"Color?"_=
					{
						"Names"_= {"--color"}
						, "Default"_= CCommandLineDefaults::fs_ColorEnabledDefault()
						, "Description"_= "Display text output with ansi colors where supported.\n"\
						"You can override behaviour with MalterlibColor env var. Set it to true or false. "\
					}
				}
			)
		;
		f_RegisterGlobalOptions
			(
				{
					"Color24Bit?"_=
					{
						"Names"_= {"--color-24bit"}
						, "Default"_= CCommandLineDefaults::fs_Color24BitEnabledDefault()
						, "Description"_= "Display text output with 24 bit ansi colors.\n"\
						"By default detected through COLORTERM. "\
						"You can override behaviour with MalterlibColor24Bit env var. Set it to true or false. "\
					}
				}
			)
		;
		f_RegisterGlobalOptions
			(
				{
					"ColorLight?"_=
					{
						"Names"_= {"--color-light"}
						, "Default"_= CCommandLineDefaults::fs_ColorLightBackgroundDefault()
						, "Description"_= "Force light background.\n"\
						"You can override behaviour with MalterlibColorLight env var. Set it to true, false or auto. "\
						"With auto a xterm secquence will be used to determine background color. "\
						"Can interfere with commands that use std input processing."
					}
				}
			)
		;
		f_RegisterGlobalOptions
			(
				{
					"BoxDrawing?"_=
					{
						"Names"_= {"--box-drawing"}
						, "Default"_= CCommandLineDefaults::fs_BoxDrawingDefault()
						, "Description"_= "Enable box drawing characters.\n"\
					}
				}
			)
		;
		f_RegisterGlobalOptions
			(
				{
					"TerminalWidth?"_=
					{
						"Names"_= {"--terminal-width"}
						, "Default"_= fg_GetSys()->f_GetEnvironmentVariable("MalterlibTerminalWidth", "-1").f_ToInt(int64(-1))
						, "Description"_= "Override detected terminal width.\n"\
						"Useful for when for whatever reason tty is not accessible. "\
					}
				}
			)
		;
		f_RegisterGlobalOptions
			(
				{
					"TerminalHeight?"_=
					{
						"Names"_= {"--terminal-height"}
						, "Default"_= fg_GetSys()->f_GetEnvironmentVariable("MalterlibTerminalHeight", "-1").f_ToInt(int64(-1))
						, "Description"_= "Override detected terminal height.\n"\
						"Useful for when for whatever reason tty is not accessible. "\
					}
				}
			)
		;
	}
}
