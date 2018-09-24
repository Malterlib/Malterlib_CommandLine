// Copyright © 2018 Nonna Holding AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

#include <Mib/Function/Function>

namespace NMib::NCommandLine
{
	#define DMibCommandLineAnsiColor_Reset "\x1B[0m"
	#define DMibCommandLineAnsiColor_Bold "\x1B[1m"
	#define DMibCommandLineAnsiColor_Reverse "\x1B[7m"
	#define DMibCommandLineAnsiColor_256(d_Color) "\x1B[38;5;" #d_Color "m"

	struct CAnsiEncoding
	{
		struct CDecodedColor
		{
			void f_Set(uint8 _Red, uint8 _Green, uint8 _Blue);
			void f_SetAnsi16(uint8 _Value);
			void f_SetAnsi256(uint8 _Value);

			uint8 m_Red = 0;
			uint8 m_Green = 0;
			uint8 m_Blue = 0;
			bool m_bEnabled = false;
		};

		static void fs_Parse
			(
				NStr::CStr const &_In
				, NFunction::TCFunction<void (NStr::CStr const &_String)> const &_fOnString
				, NFunction::TCFunction<void (CDecodedColor const &_Color, CDecodedColor const &_BGColor)> const &_fOnColor
			)
		;

		NStr::CStr static fs_StripEncoding(NStr::CStr const &_In);

		static NStr::CStr fs_Foreground16(uint8 _Color);
		static NStr::CStr fs_Background16(uint8 _Color);
		static NStr::CStr fs_Foreground256(uint8 _Color);
		static NStr::CStr fs_Background256(uint8 _Color);
		static NStr::CStr fs_ForegroundRGB(uint8 _Red, uint8 _Green, uint8 _Blue);
		static NStr::CStr fs_BackgroundRGB(uint8 _Red, uint8 _Green, uint8 _Blue);

		static ch8 const ms_Default[];

		static ch8 const ms_StatusNormal[];
		static ch8 const ms_StatusWarning[];
		static ch8 const ms_StatusError[];
		static ch8 const ms_Bold[];

		static ch8 const ms_Prompt[];
	};
}

#ifndef DMibPNoShortCuts
	using namespace NMib::NCommandLine;
	#define DAnsiColor_Reset DMibCommandLineAnsiColor_Reset
	#define DAnsiColor_Bold DMibCommandLineAnsiColor_Bold
	#define DAnsiColor_Reverse DMibCommandLineAnsiColor_Reverse
	#define DAnsiColor_256 DMibCommandLineAnsiColor_256
#endif
