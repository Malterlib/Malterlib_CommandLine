// Copyright © 2018 Nonna Holding AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

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
	};

	struct CAnsiEncoding
	{
		enum ESyntaxColor
		{
			ESyntaxColor_Number
			, ESyntaxColor_String
			, ESyntaxColor_Constant
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

		struct CLine
		{
			auto f_Tuple() const;
			bool operator == (CLine const &_Right) const;
			bool operator < (CLine const &_Right) const;

			template <typename tf_CStr>
			void f_Format(tf_CStr &o_String) const;

			NStr::CStr m_String;
			mint m_Width = 0;
		};

		CAnsiEncoding(EAnsiEncodingFlag _Flags);

		NStr::CStr f_ReEncode(NStr::CStr const &_In) const;
		NContainer::TCVector<CLine> f_LineBreak(NStr::CStr const &_String, mint _Length, EWordWrap _WordWrap = EWordWrap_Word) const;

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
		NStr::CStr::CFormat f_ForegroundRGBFormat(uint32 _RGB) const;
		NStr::CStr::CFormat f_BackgroundRGBFormat(uint32 _RGB) const;

		NStr::CStr const &f_Default() const;
		NStr::CStr const &f_Reset() const;
		NStr::CStr const &f_MoveUpperLeft() const;
		NStr::CStr::CFormat f_Move(uint32 _Row, uint32 _Column) const;
		NStr::CStr::CFormat f_MoveNextLine(uint32 _nLines = 1) const;
		NStr::CStr::CFormat f_ShowCursor(bool _bShow) const;
		NStr::CStr::CFormat f_EnableAlternativeScreenBuffer(bool _bEnable) const;

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
