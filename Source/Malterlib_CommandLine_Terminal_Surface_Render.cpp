// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <Mib/Core/Core>
#include <Mib/CommandLine/AnsiEncodingParse>
#include <Mib/String/Appender>

#include "Malterlib_CommandLine_Terminal.h"

namespace NMib::NCommandLine
{
	using namespace NStr;

	CStr CTerminalSurface::f_Render(ETerminalRenderFlag _Flags)
	{
		CStr OutputString;
		{
			CStr::CAppender StringAppender(OutputString);

			StringAppender += mp_AnsiEncoding.f_MoveUpperLeft();

			if ((_Flags & ETerminalRenderFlag::mc_NoSynchronizedUpdate) == ETerminalRenderFlag::mc_None)
				StringAppender += mp_AnsiEncoding.f_SyncronizeOutputStart();

			// Start from a known default state: earlier output may have left attributes active,
			// and the property emission below only emits non default attributes
			StringAppender += mp_AnsiEncoding.f_Default();

			umint Width = mp_Size.m_Width;
			umint Height = mp_Size.m_Height;

			auto pBuffer = mp_Buffer.f_GetArray();

			CChixelProperties LastProperties;

			uint32 LastForeground = TCLimitsInt<uint32>::mc_Max;
			uint32 LastBackground = TCLimitsInt<uint32>::mc_Max;
			uint32 LastUnderline = TCLimitsInt<uint32>::mc_Max;

			auto fSetCurrentProperties = [&]
				{
					auto Flags = pBuffer->f_GetFlags();

					auto SgrSequence = mp_AnsiEncoding.f_StartSgr(StringAppender);

					if (auto Weight = CChixelProperties::fs_Weight(Flags); Weight != CAnsiEncoding::EWeight::mc_Normal)
						SgrSequence.f_Weight(Weight);

					if (auto Underline = CChixelProperties::fs_Underline(Flags); Underline != CAnsiEncoding::EUnderline::mc_None)
						SgrSequence.f_Underline(Underline);

					if (fg_IsSet(Flags, EChixelFlag::mc_Italic))
						SgrSequence.f_Italic();

					if (fg_IsSet(Flags, EChixelFlag::mc_Strikeout))
						SgrSequence.f_Strikeout();

					// The transparent color is the terminal's own default for the channel, not black
					auto &ForegroundColor = pBuffer->m_ForegroundColor;
					if (!ForegroundColor.f_Alpha())
						LastForeground = SgrSequence.f_ForegroundDefaultDiff(LastForeground);
					else
					{
						LastForeground = SgrSequence.f_ForegroundRGBDiff
							(
								LastForeground
								, ForegroundColor.f_Red()
								, ForegroundColor.f_Green()
								, ForegroundColor.f_Blue()
							)
						;
					}
					auto &BackgroundColor = pBuffer->m_BackgroundColor;
					if (!BackgroundColor.f_Alpha())
						LastBackground = SgrSequence.f_BackgroundDefaultDiff(LastBackground);
					else
					{
						LastBackground = SgrSequence.f_BackgroundRGBDiff
							(
								LastBackground
								, BackgroundColor.f_Red()
								, BackgroundColor.f_Green()
								, BackgroundColor.f_Blue()
							)
						;
					}
					auto &UnderlineColor = pBuffer->m_UnderlineColor;
					if (!UnderlineColor.f_Alpha())
						LastUnderline = SgrSequence.f_UnderlineDefaultDiff(LastUnderline);
					else
					{
						LastUnderline = SgrSequence.f_UnderlineRGBDiff
							(
								LastUnderline
								, UnderlineColor.f_Red()
								, UnderlineColor.f_Green()
								, UnderlineColor.f_Blue()
							)
						;
					}

					LastProperties.m_ForegroundColor = ForegroundColor;
					LastProperties.m_BackgroundColor = BackgroundColor;
					LastProperties.m_UnderlineColor = UnderlineColor;
					LastProperties.m_Flags = Flags;
				}
			;

			if ((_Flags & ETerminalRenderFlag::mc_ResetAtEndOfLine) == ETerminalRenderFlag::mc_None)
				fSetCurrentProperties();

			for (umint iYCord = 0; iYCord < Height; ++iYCord)
			{
				if ((_Flags & ETerminalRenderFlag::mc_ResetAtEndOfLine) != ETerminalRenderFlag::mc_None)
					fSetCurrentProperties();

				for (umint iXCord = 0; iXCord < Width; ++iXCord)
				{
					auto Flags = pBuffer->f_GetFlags();
					auto &ForegroundColor = pBuffer->m_ForegroundColor;
					auto &BackgroundColor = pBuffer->m_BackgroundColor;
					auto &UnderlineColor = pBuffer->m_UnderlineColor;
					{
						auto SgrSequence = mp_AnsiEncoding.f_StartSgr(StringAppender);

						if (Flags != LastProperties.m_Flags)
						{
							if (auto Weight = CChixelProperties::fs_Weight(Flags); Weight != LastProperties.f_Weight())
								SgrSequence.f_Weight(Weight);

							if (auto Underline = CChixelProperties::fs_Underline(Flags); Underline != LastProperties.f_Underline())
								SgrSequence.f_Underline(Underline);

							if (fg_IsSet(Flags, EChixelFlag::mc_Italic) && !fg_IsSet(LastProperties.m_Flags, EChixelFlag::mc_Italic))
								SgrSequence.f_Italic();
							else if (!fg_IsSet(Flags, EChixelFlag::mc_Italic) && fg_IsSet(LastProperties.m_Flags, EChixelFlag::mc_Italic))
								SgrSequence.f_NotItalic();

							if (fg_IsSet(Flags, EChixelFlag::mc_Strikeout) && !fg_IsSet(LastProperties.m_Flags, EChixelFlag::mc_Strikeout))
								SgrSequence.f_Strikeout();
							else if (!fg_IsSet(Flags, EChixelFlag::mc_Strikeout) && fg_IsSet(LastProperties.m_Flags, EChixelFlag::mc_Strikeout))
								SgrSequence.f_NotStrikeout();
						}

						// The transparent color is the terminal's own default for the channel, not black
						if (ForegroundColor != LastProperties.m_ForegroundColor)
						{
							if (!ForegroundColor.f_Alpha())
								LastForeground = SgrSequence.f_ForegroundDefaultDiff(LastForeground);
							else
							{
								LastForeground = SgrSequence.f_ForegroundRGBDiff
									(
										LastForeground
										, ForegroundColor.f_Red()
										, ForegroundColor.f_Green()
										, ForegroundColor.f_Blue()
									)
								;
							}
						}

						if (BackgroundColor != LastProperties.m_BackgroundColor)
						{
							if (!BackgroundColor.f_Alpha())
								LastBackground = SgrSequence.f_BackgroundDefaultDiff(LastBackground);
							else
							{
								LastBackground = SgrSequence.f_BackgroundRGBDiff
									(
										LastBackground
										, BackgroundColor.f_Red()
										, BackgroundColor.f_Green()
										, BackgroundColor.f_Blue()
									)
								;
							}
						}

						if (UnderlineColor != LastProperties.m_UnderlineColor)
						{
							if (!UnderlineColor.f_Alpha())
								LastUnderline = SgrSequence.f_UnderlineDefaultDiff(LastUnderline);
							else
							{
								LastUnderline = SgrSequence.f_UnderlineRGBDiff
									(
										LastUnderline
										, UnderlineColor.f_Red()
										, UnderlineColor.f_Green()
										, UnderlineColor.f_Blue()
									)
								;
							}
						}
					}

					StringAppender.f_AddUnicodeChar(pBuffer->f_GetCharacter());
	#if DCommandLineSupportOverflow
					if (pBuffer->m_bHasOverflow)
					{
						if (NStr::CStr const *pOverflow = fp_FindOverflow(iYCord * Width + iXCord))
							StringAppender += *pOverflow;
					}
	#endif
					LastProperties.m_ForegroundColor = ForegroundColor;
					LastProperties.m_BackgroundColor = BackgroundColor;
					LastProperties.m_UnderlineColor = UnderlineColor;
					LastProperties.m_Flags = Flags;
					++pBuffer;
				}

				if ((_Flags & ETerminalRenderFlag::mc_ResetAtEndOfLine) != ETerminalRenderFlag::mc_None)
				{
					StringAppender += mp_AnsiEncoding.f_Default();

					// The terminal is back at default colors; invalidate the caches so the next row re-emits its colors
					LastForeground = TCLimitsInt<uint32>::mc_Max;
					LastBackground = TCLimitsInt<uint32>::mc_Max;
					LastUnderline = TCLimitsInt<uint32>::mc_Max;
				}

				// Rows end in a plain line feed: this output is text for a cooked terminal, a file or a
				// comparison, where an escape would not be a line break. A frame drawn on a terminal in
				// raw mode goes through the diff render, which positions every row itself
				if (iYCord != Height - 1)
					StringAppender += "\n";
			}

			if ((_Flags & ETerminalRenderFlag::mc_ResetAtEndOfLine) == ETerminalRenderFlag::mc_None)
				StringAppender += mp_AnsiEncoding.f_Default();

			if ((_Flags & ETerminalRenderFlag::mc_NoSynchronizedUpdate) == ETerminalRenderFlag::mc_None)
				StringAppender += mp_AnsiEncoding.f_SyncronizeOutputFinish();
		}

		return OutputString;
	}
}
