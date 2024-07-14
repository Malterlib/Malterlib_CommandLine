// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

#include <Mib/Function/Function>
#include <Mib/Storage/Variant>
#include <Mib/Storage/Tuple>
#include <Mib/CommandLine/AnsiEncoding>

namespace NMib::NCommandLine
{
	struct CAnsiEncodingParse
	{
		struct CDecodedColor
		{
			void f_Set(uint8 _Red, uint8 _Green, uint8 _Blue);
			void f_SetAnsi16(uint8 _Value);
			void f_SetAnsi256(uint8 _Value);

			auto operator <=> (CDecodedColor const &_Right) const noexcept = default;

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

		struct CUnderlineColor : public CDecodedColor
		{
		};

		struct CWeight
		{
			CAnsiEncoding::EWeight m_Weight = CAnsiEncoding::EWeight::mc_Normal;

			auto operator <=> (CWeight const &_Other) const noexcept = default;
		};

		struct CUnderline
		{
			CAnsiEncoding::EUnderline m_Underline = CAnsiEncoding::EUnderline::mc_None;

			auto operator <=> (CUnderline const &_Other) const noexcept = default;
		};

		struct CItalic
		{
			bool m_bEnabled = false;

			auto operator <=> (CItalic const &_Other) const noexcept = default;
		};

		struct CStrikeout
		{
			bool m_bEnabled = false;

			auto operator <=> (CStrikeout const &_Other) const noexcept = default;
		};

		struct CReset
		{
			auto operator <=> (CReset const &_Other) const noexcept = default;
		};

		using CPropertyChange = NStorage::TCVariant<CForegroundColor, CBackgroundColor, CUnderlineColor, CWeight, CUnderline, CItalic, CStrikeout, CReset>;

		struct CParseState
		{
			CAnsiEncoding::EWeight m_Weight = CAnsiEncoding::EWeight::mc_Normal;
			CAnsiEncoding::EUnderline m_Underline = CAnsiEncoding::EUnderline::mc_None;
			bool m_bItalic = false;
			bool m_bStrikeout = false;
			bool m_bAborted = false;
			int32 m_LastForeground = -1;
			CDecodedColor m_CurrentColor;
			CDecodedColor m_CurrentColorBG;
			CDecodedColor m_CurrentColorUnderline;
		};

		static void fs_Parse
			(
				NStr::CStr const &_In
				, NFunction::TCFunction<bool (NStr::CStr const &_String)> const &_fOnString
				, NFunction::TCFunction<void (CPropertyChange const &_Property)> const &_fOnPropertyChange
				, CParseState *_pParseState = nullptr
			)
		;

		static NStr::CStr fs_StripEncoding(NStr::CStr const &_In);
		static umint fs_RenderedStrLen(NStr::CStr const &_String);
	};
}

#include "Malterlib_CommandLine_AnsiEncodingParse.hpp"

#ifndef DMibPNoShortCuts
	using namespace NMib::NCommandLine;
#endif
