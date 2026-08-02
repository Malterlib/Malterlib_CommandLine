// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

#include <Mib/Function/Function>
#include <Mib/Storage/Optional>
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

		// The active properties at a point in a parsed stream; unset values are the defaults
		struct CActiveProperties
		{
			auto operator <=> (CActiveProperties const &_Right) const noexcept = default;

			NStorage::TCOptional<CBackgroundColor> m_BackgroundColor;
			NStorage::TCOptional<CForegroundColor> m_ForegroundColor;
			NStorage::TCOptional<CUnderlineColor> m_UnderlineColor;
			NStorage::TCOptional<CWeight> m_Weight;
			NStorage::TCOptional<CUnderline> m_Underline;
			NStorage::TCOptional<CItalic> m_Italic;
			NStorage::TCOptional<CStrikeout> m_Strikeout;
		};

		// Pre-parsed, line-broken text: runs are pooled flat and lines reference ranges of them.
		// Run text is plain (no escape sequences), so rendering it needs no ANSI parsing.
		struct CParsedRun
		{
			NStr::CStrPtr m_Text; // View into CParsedText::m_Source (ellipsis runs view constant storage)
			CActiveProperties m_Properties;
		};

		struct CParsedLine
		{
			umint m_iFirstRun = 0;
			umint m_nRuns = 0;
			umint m_Width = 0; // Cells
		};

		struct CParsedText
		{
			NStr::CStr m_Source; // Stripped source text the run views point into
			NContainer::TCVector<CParsedRun> m_Runs;
			NContainer::TCVector<CParsedLine> m_Lines;
		};

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

		// _fOnString receives contiguous views of _In without escape sequences. The views reference
		// _In's storage (they are not NUL terminated) and are only valid during the callback.
		static void fs_Parse
			(
				NStr::CStr const &_In
				, NFunction::TCFunction<bool (NStr::CStrPtr const &_String)> const &_fOnString
				, NFunction::TCFunction<void (CPropertyChange const &_Property)> const &_fOnPropertyChange
				, CParseState *_pParseState = nullptr
			)
		;

		static NStr::CStr fs_StripEncoding(NStr::CStr const &_In);
		static umint fs_RenderedStrLen(NStr::CStr const &_String);

		// Line breaks _String like CAnsiEncoding::f_LineBreak but emits pre-parsed runs instead of
		// strings with re-encoded escape sequences, so the text is parsed exactly once between
		// layout and rendering; o_Text is cleared without releasing its buffers. The run views
		// stay valid while o_Text.m_Source keeps its storage: reassigning it (including a later
		// fs_LineBreak into the same o_Text) invalidates them
		static void fs_LineBreak
			(
				NStr::CStr const &_String
				, umint _Length
				, CAnsiEncoding::EWordWrap _WordWrap
				, CParsedText &o_Text
			)
		;
	};
}

#include "Malterlib_CommandLine_AnsiEncodingParse.hpp"

#ifndef DMibPNoShortCuts
	using namespace NMib::NCommandLine;
#endif
