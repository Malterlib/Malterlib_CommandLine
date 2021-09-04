// Copyright © 2018 Nonna Holding AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

#include <Mib/Function/Function>
#include <Mib/Storage/Variant>
#include <Mib/Storage/Tuple>

namespace NMib::NCommandLine
{
	struct CAnsiEncodingParse
	{
		struct CDecodedColor
		{
			void f_Set(uint8 _Red, uint8 _Green, uint8 _Blue);
			void f_SetAnsi16(uint8 _Value);
			void f_SetAnsi256(uint8 _Value);

			auto operator <=> (CDecodedColor const &_Right) const = default;

			uint8 m_Red = 0;
			uint8 m_Green = 0;
			uint8 m_Blue = 0;
			bool m_bEnabled = false;
		};

		struct CBackgroundColor : public CDecodedColor
		{
		};

		struct CForegroundColor : public CDecodedColor
		{
		};

		struct CBold
		{
			bool m_bEnabled = false;

			auto operator <=> (CBold const &_Other) const = default;
		};

		struct CItalic
		{
			bool m_bEnabled = false;

			auto operator <=> (CItalic const &_Other) const = default;
		};

		struct CReset
		{
			auto operator <=> (CReset const &_Other) const = default;
		};

		using CPropertyChange = NStorage::TCVariant<CForegroundColor, CBackgroundColor, CBold, CItalic, CReset>;

		static void fs_Parse
			(
				NStr::CStr const &_In
				, NFunction::TCFunction<bool (NStr::CStr const &_String)> const &_fOnString
				, NFunction::TCFunction<void (CPropertyChange const &_Property)> const &_fOnPropertyChange
			)
		;

		static NStr::CStr fs_StripEncoding(NStr::CStr const &_In);
		static mint fs_RenderedStrLen(NStr::CStr const &_String);
	};
}

#include "Malterlib_CommandLine_AnsiEncodingParse.hpp"

#ifndef DMibPNoShortCuts
	using namespace NMib::NCommandLine;
#endif
