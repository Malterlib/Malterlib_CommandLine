// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

namespace NMib::NCommandLine
{
	enum EAnsiEncodingFlag
	{
		EAnsiEncodingFlag_None = 0
		, EAnsiEncodingFlag_Color = DMibBit(1)
		, EAnsiEncodingFlag_Color24Bit = DMibBit(2)
		, EAnsiEncodingFlag_ColorLightBackground = DMibBit(3)
		, EAnsiEncodingFlag_BoxDrawing = DMibBit(4)
		, EAnsiEncodingFlag_ColorSgrUsesSemiColon = DMibBit(5)

		, EAnsiEncodingFlag_AllFeatures = EAnsiEncodingFlag_Color | EAnsiEncodingFlag_Color24Bit | EAnsiEncodingFlag_BoxDrawing
	};

	struct CAnsiEncoding
	{
		enum ESyntaxColor
		{
			ESyntaxColor_Number
			, ESyntaxColor_String
			, ESyntaxColor_Constant
			, ESyntaxColor_Comment
			, ESyntaxColor_MemberVariable
		};

		enum EWordWrap
		{
			EWordWrap_None
			, EWordWrap_Word
			, EWordWrap_WordEllipsis
			, EWordWrap_Character
			, EWordWrap_CharacterEllipsis
			, EWordWrap_Ellipsis
		};

		enum class EComprehensiveKeyFlags : uint8
		{
			mc_None = 0
			, mc_DisambiguateEscapeCodes = fg_Bit(0)
			, mc_ReportEventTypes = fg_Bit(1)
			, mc_ReportAlternateKeys = fg_Bit(2)
			, mc_ReportAllKeysAsEscapeCodes = fg_Bit(3)
			, mc_ReportAssociatedText = fg_Bit(4)

			// Key event handling reports (Unbroken extension): every physical key arrives in the
			// CSI u form, legacy-only functional keys in the U+F500 block, and the terminal defers
			// its own marked keybindings, forwarding the key with a report id and only running
			// them when the application reports the event as unhandled
			, mc_ReportKeyEventHandling = fg_Bit(7)
		};

		enum class EWeight : uint32
		{
			mc_Normal
			, mc_Bold
			, mc_Dim
			, mc_Shadowed
		};

		enum class EUnderline : uint32
		{
			mc_None
			, mc_Solid
			, mc_Double
			, mc_Wavy
			, mc_Dotted
			, mc_Dashed
		};

		struct CLine
		{
			bool operator == (CLine const &_Right) const noexcept = default;
			auto operator <=> (CLine const &_Right) const noexcept = default;

			template <typename tf_CStr>
			void f_Format(tf_CStr &o_String) const;

			NStr::CStr m_String;
			umint m_Width = 0;
		};

		struct CSgrSequence
		{
			CSgrSequence(NStr::CStr::CAppender *_pAppender, EAnsiEncodingFlag _Flags);
			~CSgrSequence();

			void f_ForegroundRGB(uint8 _Red, uint8 _Green, uint8 _Blue);
			void f_BackgroundRGB(uint8 _Red, uint8 _Green, uint8 _Blue);
			void f_UnderlineRGB(uint8 _Red, uint8 _Green, uint8 _Blue);

			uint32 f_ForegroundRGBDiff(uint32 _Previous, uint8 _Red, uint8 _Green, uint8 _Blue);
			uint32 f_BackgroundRGBDiff(uint32 _Previous, uint8 _Red, uint8 _Green, uint8 _Blue);
			uint32 f_UnderlineRGBDiff(uint32 _Previous, uint8 _Red, uint8 _Green, uint8 _Blue);

			void f_Weight(EWeight _Weight);
			void f_Underline(EUnderline _Underline);
			void f_Italic();
			void f_NotItalic();
			void f_Strikeout();
			void f_NotStrikeout();

		private:
			void fp_StartSequence();

			NStr::CStr::CAppender *mp_pAppender = nullptr;
			EAnsiEncodingFlag mp_Flags = EAnsiEncodingFlag_None;
			bool mp_bFirst = true;
		};

		CAnsiEncoding(EAnsiEncodingFlag _Flags);

		NStr::CStr f_ReEncode(NStr::CStr const &_In) const;
		NContainer::TCVector<CLine> f_LineBreak(NStr::CStr const &_String, umint _Length, EWordWrap _WordWrap = EWordWrap_Word) const;

		// Fills o_Lines instead of returning a new vector so callers can reuse its capacity across
		// calls; o_Lines is cleared without releasing its buffer
		void f_LineBreak(NStr::CStr const &_String, umint _Length, EWordWrap _WordWrap, NContainer::TCVector<CLine> &o_Lines) const;

		NStr::CStr f_Foreground16(uint8 _Color) const;
		NStr::CStr f_Background16(uint8 _Color) const;
		NStr::CStr f_Foreground256(uint8 _Color) const;
		NStr::CStr f_Background256(uint8 _Color) const;

		NStr::CStr f_ForegroundRGB(uint8 _Red, uint8 _Green, uint8 _Blue) const;
		NStr::CStr f_BackgroundRGB(uint8 _Red, uint8 _Green, uint8 _Blue) const;
		NStr::CStr f_ForegroundRGB(uint32 _RGB) const;
		NStr::CStr f_BackgroundRGB(uint32 _RGB) const;

		NStr::CStr f_SyncronizeOutputStart() const;
		NStr::CStr f_SyncronizeOutputFinish() const;

		NStr::CStr::CFormat f_ForegroundRGBFormat(uint8 _Red, uint8 _Green, uint8 _Blue) const;
		NStr::CStr::CFormat f_BackgroundRGBFormat(uint8 _Red, uint8 _Green, uint8 _Blue) const;
		NStr::CStr::CFormat f_UnderlineRGBFormat(uint8 _Red, uint8 _Green, uint8 _Blue) const;
		NStr::CStr::CFormat f_ForegroundRGBFormat(uint32 _RGB) const;
		NStr::CStr::CFormat f_BackgroundRGBFormat(uint32 _RGB) const;
		NStr::CStr::CFormat f_UnderlineRGBFormat(uint32 _RGB) const;

		CSgrSequence f_StartSgr(NStr::CStr::CAppender &_Appender);

		NStr::CStr const &f_Default() const;
		NStr::CStr const &f_Reset() const;
		NStr::CStr const &f_MoveUpperLeft() const;
		NStr::CFStr24 f_Move(uint32 _Row, uint32 _Column) const;
		NStr::CStr::CFormat f_MoveToColumn(uint32 _Column) const;
		NStr::CStr::CFormat f_MovePreviousLine(uint32 _nLines = 1) const;
		NStr::CFStr24 f_MoveNextLine(uint32 _nLines = 1) const;
		NStr::CStr const &f_ClearToEndOfScreen() const;
		NStr::CFStr24 f_ShowCursor(bool _bShow) const;
		NStr::CFStr24 f_EnableAlternativeScreenBuffer(bool _bEnable) const;
		NStr::CFStr24 f_EnableMouseReporting(bool _bEnable) const;
		NStr::CFStr24 f_PushComprehensiveKeyHandling(EComprehensiveKeyFlags _KeyHandlingFlags) const;
		NStr::CStr const &f_PopComprehensiveKeyHandling() const;

		// Support probe: a terminal implementing the comprehensive keyboard protocol replies with
		// CSI ? flags u; others stay silent. Pushing mc_ReportKeyEventHandling and checking that
		// the reply retains it is how support for the handling report extension is detected
		NStr::CStr const &f_QueryComprehensiveKeyHandling() const;

		// Answers a key event that carried a handling report id: unhandled runs the terminal's
		// deferred keybinding, handled drops it
		NStr::CFStr24 f_ReportKeyEventHandled(uint16 _ID, bool _bHandled) const;

		NStr::CStr const &f_StatusNormal() const;
		NStr::CStr f_StatusNormal(NStr::CStr const &_ToWrap) const;
		NStr::CStr const &f_StatusWarning() const;
		NStr::CStr f_StatusWarning(NStr::CStr const &_ToWrap) const;
		NStr::CStr const &f_StatusError() const;
		NStr::CStr f_StatusError(NStr::CStr const &_ToWrap) const;
		NStr::CStr const &f_Bold() const;
		NStr::CStr const &f_NotBold() const;
		NStr::CStr const &f_Italic() const;
		NStr::CStr const &f_NotItalic() const;
		NStr::CStr const &f_Strikeout() const;
		NStr::CStr const &f_NotStrikeout() const;

		NStr::CStr const &f_Weight(EWeight _Weight) const;
		NStr::CStr const &f_Underline(EUnderline _Underline) const;

		NStr::CStr f_Bold(NStr::CStr const &_ToWrap) const;
		NStr::CStr const &f_Prompt() const;
		NStr::CStr f_Prompt(NStr::CStr const &_ToWrap) const;

		NStr::CStr f_SyntaxColor(ESyntaxColor _Color) const;
		template <typename tf_CToWrap>
		NStr::CStr f_SyntaxColor(ESyntaxColor _Color, tf_CToWrap const &_ToWrap) const;

		NStr::CStr f_CreateSemiUniqueColor(NStr::CStr const &_Data) const;

		template <typename tf_CType>
		NStr::CStr f_CreateSemiUniqueColor(tf_CType const &_Data) const;

		template <typename tf_CType>
		NStr::CStr f_ColorSemiUnique(tf_CType const &_Data) const;

		bool f_Color() const;

		EAnsiEncodingFlag f_Flags() const;

	private:
		EAnsiEncodingFlag mp_Flags = EAnsiEncodingFlag_None;
		NStr::CStr mp_StatusNormal;
		NStr::CStr mp_StatusWarning;
		NStr::CStr mp_StatusError;
		NStr::CStr mp_Prompt;
	};
}

#ifndef DMibPNoShortCuts
	using namespace NMib::NCommandLine;
#endif

#include "Malterlib_CommandLine_AnsiEncoding.hpp"
