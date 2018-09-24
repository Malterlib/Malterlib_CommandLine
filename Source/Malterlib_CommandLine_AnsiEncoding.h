// Copyright © 2018 Nonna Holding AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

#include <Mib/Function/Function>

namespace NMib::NCommandLine
{
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
	};
}

#ifndef DMibPNoShortCuts
	using namespace NMib::NCommandLine;
#endif
