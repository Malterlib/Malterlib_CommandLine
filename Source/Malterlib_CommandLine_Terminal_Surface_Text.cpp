// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <Mib/Core/Core>
#include <Mib/CommandLine/AnsiEncodingParse>

#include "Malterlib_CommandLine_Terminal.h"

namespace NMib::NCommandLine
{
	using namespace NStr;

	int32 CTerminalSurface::f_TextRun
		(
			CTerminalPoint _Position
			, CStrPtr const &_Text
			, CSetChixelProperties const &_Properties
			, CTerminalArea const &_ClipArea
		)
	{
		CTerminalArea SurfaceArea = f_Area();

		CTerminalArea ClipArea = _ClipArea;
		if (!ClipArea.f_IsValid())
			ClipArea = SurfaceArea;

		auto pBuffer = mp_Buffer.f_GetArray();
		int32 SurfaceWidth = mp_Size.m_Width;

		CTerminalPoint OutputPosition = _Position;
		CChixel *pPreviousChixel = nullptr;
		auto ClipLowerRight = ClipArea.f_LowerRight();

		for (auto iParse = _Text.f_GetUnicodeIterator(); iParse; ++iParse)
		{
			ch32 Char = *iParse;

#if DCommandLineSupportOverflow
			if (fg_CharIsCombining(Char))
			{
				if (pPreviousChixel)
				{
					auto &Overflow = mp_OverflowStrings[umint(pPreviousChixel - pBuffer)];

					// The entry may linger from an earlier occupant of the cell
					if (!pPreviousChixel->m_bHasOverflow)
					{
						pPreviousChixel->m_bHasOverflow = true;
						Overflow.f_Clear();
					}

					Overflow.f_AddUnicodeChar(Char);
				}
				continue;
			}
#endif
			if (SurfaceArea.f_Contains(OutputPosition) && ClipArea.f_Contains(OutputPosition))
			{
				auto &Chixel = pBuffer[OutputPosition.m_CoordY * SurfaceWidth + OutputPosition.m_CoordX];
				pPreviousChixel = &Chixel;

				if (_Properties.m_ChixelFlags)
					Chixel.f_SetAll(Char, *_Properties.m_ChixelFlags);
				else
					Chixel.f_SetCharacterAndEndpointType(Char);

				if (_Properties.m_ForegroundColor)
					Chixel.m_ForegroundColor = *_Properties.m_ForegroundColor;

				if (_Properties.m_BackgroundColor)
					Chixel.m_BackgroundColor = *_Properties.m_BackgroundColor;

				if (_Properties.m_UnderlineColor)
					Chixel.m_UnderlineColor = *_Properties.m_UnderlineColor;
			}
			else
				pPreviousChixel = nullptr;

			++OutputPosition.m_CoordX;

			if (OutputPosition.m_CoordX >= ClipLowerRight.m_CoordX)
				break;
		}

		return OutputPosition.m_CoordX;
	}

	CTerminalArea CTerminalSurface::f_Text(CStr const &_String, CTextOptions const &_Options)
	{
		CTerminalArea DestinationArea = _Options.m_DestinationArea;

		if (!DestinationArea.f_IsValid())
			DestinationArea.m_Size = mp_Size;

		CTerminalArea SurfaceArea = f_Area();

		if (!SurfaceArea.f_Intersects(DestinationArea))
			return CTerminalArea{DestinationArea.m_UpperLeft, {0, 0}};

		auto pBuffer = mp_Buffer.f_GetArray();

		CTerminalPoint OutputPosition = DestinationArea.m_UpperLeft;
		CTerminalPoint StartPosition = OutputPosition;

		int32 SurfaceHeight = mp_Size.m_Height;
		int32 SurfaceWidth = mp_Size.m_Width;

		auto DestinationLowerRight = DestinationArea.f_LowerRight();

		if (DestinationArea.m_Size.m_Width < 1)
			return CTerminalArea{DestinationArea.m_UpperLeft, {0, 0}};

		// The ellipsis word wrap modes require at least three cells to make progress
		if ((_Options.m_WordWrap == CAnsiEncoding::EWordWrap_WordEllipsis || _Options.m_WordWrap == CAnsiEncoding::EWordWrap_CharacterEllipsis) && DestinationArea.m_Size.m_Width < 3)
			return CTerminalArea{DestinationArea.m_UpperLeft, {0, 0}};

		mp_AnsiEncoding.f_LineBreak(_String, DestinationArea.m_Size.m_Width, _Options.m_WordWrap, mp_TextLines);
		auto &Lines = mp_TextLines;

		CTerminalArea TextArea;
		TextArea.m_UpperLeft = DestinationArea.m_UpperLeft;

		if ((_Options.m_Aligment & ETerminalTextAlignment::mc_VerticalCenter) != ETerminalTextAlignment::mc_None)
		{
			auto Offset = (DestinationArea.m_Size.m_Height - int32(Lines.f_GetLen())) / 2;
			TextArea.m_UpperLeft.m_CoordY += Offset;
			OutputPosition.m_CoordY += Offset;
		}

		CSetChixelProperties NextProperties = _Options.m_Properties;

		int32 MaxWidth = 0;
		for (auto &Line : Lines)
		{
			CChixel *pPreviousChixel = nullptr;

			OutputPosition.m_CoordX = StartPosition.m_CoordX;

			MaxWidth = fg_Max(MaxWidth, Line.m_Width);

			if ((_Options.m_Aligment & ETerminalTextAlignment::mc_HorizontalCenter) != ETerminalTextAlignment::mc_None)
				OutputPosition.m_CoordX += (DestinationArea.m_Size.m_Width - int32(Line.m_Width)) / 2;

			CAnsiEncodingParse::fs_Parse
				(
					Line.m_String
					, [&](CStrPtr const &_String) -> bool
					{
						for (auto iParse = _String.f_GetUnicodeIterator(); iParse; ++iParse)
						{
							if (SurfaceArea.f_Contains(OutputPosition) && DestinationArea.f_Contains(OutputPosition))
							{
								auto &Chixel = pBuffer[OutputPosition.m_CoordY * SurfaceWidth + OutputPosition.m_CoordX];
								ch32 Char = *iParse;

#if DCommandLineSupportOverflow
								if (fg_CharIsCombining(Char))
								{
									if (pPreviousChixel)
									{
										auto &Overflow = mp_OverflowStrings[umint(pPreviousChixel - pBuffer)];

										// The entry may linger from an earlier occupant of the cell
										if (!pPreviousChixel->m_bHasOverflow)
										{
											pPreviousChixel->m_bHasOverflow = true;
											Overflow.f_Clear();
										}

										Overflow.f_AddUnicodeChar(Char);
									}
									continue;
								}
#endif
								// Every codepoint is stored as one single width cell; double width
								// glyphs (East Asian wide, most emoji) are not supported yet and
								// need display width aware line breaking plus a continuation cell
								pPreviousChixel = &Chixel;

								if (NextProperties.m_ChixelFlags)
									Chixel.f_SetAll(Char, *NextProperties.m_ChixelFlags);
								else
									Chixel.f_SetCharacterAndEndpointType(Char);

								if (NextProperties.m_ForegroundColor)
									Chixel.m_ForegroundColor = *NextProperties.m_ForegroundColor;

								if (NextProperties.m_BackgroundColor)
									Chixel.m_BackgroundColor = *NextProperties.m_BackgroundColor;

								if (NextProperties.m_UnderlineColor)
									Chixel.m_UnderlineColor = *NextProperties.m_UnderlineColor;
							}
							else
								pPreviousChixel = nullptr;

							++OutputPosition.m_CoordX;

							if (OutputPosition.m_CoordX >= DestinationLowerRight.m_CoordX)
								return false;
						}

						return true;
					}
					, [&](CAnsiEncodingParse::CPropertyChange const &_Change)
					{
						if ((_Options.m_Flags & ETerminalTextRenderFlag::mc_StripColor) != ETerminalTextRenderFlag::mc_None)
							return;

						_Change.f_Visit
							(
								[&]<typename tf_CType>(tf_CType const &_Change)
								{
									if constexpr (NTraits::cIsSame<tf_CType, CAnsiEncodingParse::CReset>)
									{
										NextProperties.m_ForegroundColor.f_Clear();
										NextProperties.m_BackgroundColor.f_Clear();
										NextProperties.m_UnderlineColor.f_Clear();
										NextProperties.m_ChixelFlags.f_Clear();
									}
									else if constexpr (NTraits::cIsSame<tf_CType, CAnsiEncodingParse::CWeight>)
										NextProperties.f_Weight(_Change.m_Weight);
									else if constexpr (NTraits::cIsSame<tf_CType, CAnsiEncodingParse::CUnderline>)
										NextProperties.f_Underline(_Change.m_Underline);
									else if constexpr (NTraits::cIsSame<tf_CType, CAnsiEncodingParse::CItalic>)
										NextProperties.f_ChangeFlags(EChixelFlag::mc_Italic, _Change.m_bEnabled ? EChixelFlag::mc_Italic : EChixelFlag::mc_None);
									else if constexpr (NTraits::cIsSame<tf_CType, CAnsiEncodingParse::CStrikeout>)
										NextProperties.f_ChangeFlags(EChixelFlag::mc_Strikeout, _Change.m_bEnabled ? EChixelFlag::mc_Strikeout : EChixelFlag::mc_None);
									else if constexpr (NTraits::cIsSame<tf_CType, CAnsiEncodingParse::CForegroundColor>)
										NextProperties.m_ForegroundColor = fg_ToTerminalColor(_Change);
									else if constexpr (NTraits::cIsSame<tf_CType, CAnsiEncodingParse::CBackgroundColor>)
										NextProperties.m_BackgroundColor = fg_ToTerminalColor(_Change);
									else if constexpr (NTraits::cIsSame<tf_CType, CAnsiEncodingParse::CUnderlineColor>)
										NextProperties.m_UnderlineColor = fg_ToTerminalColor(_Change);
									else
										static_assert(false);
								}
							)
						;
					}
				)
			;

			++OutputPosition.m_CoordY;
			if (OutputPosition.m_CoordY >= DestinationLowerRight.m_CoordY || OutputPosition.m_CoordY >= SurfaceHeight)
				break;
		}

		if ((_Options.m_Aligment & ETerminalTextAlignment::mc_HorizontalCenter) != ETerminalTextAlignment::mc_None)
			TextArea.m_UpperLeft.m_CoordX += (DestinationArea.m_Size.m_Width - int32(MaxWidth)) / 2;

		TextArea.m_Size.m_Width = MaxWidth;
		TextArea.m_Size.m_Height = Lines.f_GetLen();

		return TextArea;
	}
}
