// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <Mib/Core/Core>
#include <Mib/CommandLine/AnsiEncodingParse>
#include <Mib/String/Appender>

#include "Malterlib_CommandLine_Terminal.h"

namespace NMib::NCommandLine
{
	using namespace NStr;

	CStr CTerminalSurface::f_DiffRender(CTerminalSurface const &_Previous, ETerminalRenderFlag _Flags)
	{
		DMibRequire(_Previous.mp_Size == mp_Size);

		CStr OutputString;
		{
			CStr::CAppender StringAppender(OutputString);

			if ((_Flags & ETerminalRenderFlag::mc_NoSynchronizedUpdate) == ETerminalRenderFlag::mc_None)
				StringAppender += mp_AnsiEncoding.f_SyncronizeOutputStart();

			StringAppender += mp_AnsiEncoding.f_Default();

			umint Width = mp_Size.m_Width;
			umint Height = mp_Size.m_Height;

			auto pBuffer = mp_Buffer.f_GetArray();
			auto pPreviousBuffer = _Previous.mp_Buffer.f_GetArray();

			CChixelProperties LastProperties;

			bool bFirstForgroundColor = true;
			bool bFirstBackgroundColor = true;
			bool bFirstUnderlineColor = true;

			uint32 LastForeground = TCLimitsInt<uint32>::mc_Max;
			uint32 LastBackground = TCLimitsInt<uint32>::mc_Max;
			uint32 LastUnderline = TCLimitsInt<uint32>::mc_Max;

			// Start outside any valid coordinate so the first changed cell always emits a cursor move
			umint iYCoordLast = TCLimitsInt<umint>::mc_Max;
			umint iXCoordLast = TCLimitsInt<umint>::mc_Max;

			for (umint iYCoord = 0; iYCoord < Height; ++iYCoord)
			{
				for (umint iXCoord = 0; iXCoord < Width; ++iXCoord)
				{
					bool bEqual = *pBuffer == *pPreviousBuffer;

#if DCommandLineSupportOverflow
					// The flags already compared equal above; when both cells carry overflow the
					// strings in the two surfaces decide
					if (bEqual && pBuffer->m_bHasOverflow)
					{
						NStr::CStr const *pOverflow = fp_FindOverflow(iYCoord * Width + iXCoord);
						NStr::CStr const *pPreviousOverflow = _Previous.fp_FindOverflow(iYCoord * Width + iXCoord);

						bEqual = pOverflow && pPreviousOverflow
							? *pOverflow == *pPreviousOverflow
							: pOverflow == pPreviousOverflow
						;
					}
#endif

					if (bEqual)
					{
						++pBuffer;
						++pPreviousBuffer;
						continue;
					}

					if
					(
						(_Flags & ETerminalRenderFlag::mc_ResetAtEndOfLine) != ETerminalRenderFlag::mc_None
						&& iYCoordLast != TCLimitsInt<umint>::mc_Max
						&& iYCoordLast != iYCoord
					)
					{
						// Close the previous row in default state and invalidate the caches so the
						// new row re-emits its properties
						StringAppender += mp_AnsiEncoding.f_Default();

						LastProperties = CChixelProperties();
						bFirstForgroundColor = true;
						bFirstBackgroundColor = true;
						bFirstUnderlineColor = true;
						LastForeground = TCLimitsInt<uint32>::mc_Max;
						LastBackground = TCLimitsInt<uint32>::mc_Max;
						LastUnderline = TCLimitsInt<uint32>::mc_Max;
					}

					if (iYCoordLast != iYCoord || iXCoord != (iXCoordLast + 1))
						StringAppender += mp_AnsiEncoding.f_Move(iYCoord, iXCoord);

					iYCoordLast = iYCoord;
					iXCoordLast = iXCoord;

					auto Flags = pBuffer->f_GetFlags();
					auto ForegroundColor = pBuffer->m_ForegroundColor;
					auto BackgroundColor = pBuffer->m_BackgroundColor;
					auto UnderlineColor = pBuffer->m_UnderlineColor;
					{
						auto SgrSequence = mp_AnsiEncoding.f_StartSgr(StringAppender);

						if (Flags != LastProperties.m_Flags)
						{
							auto Weight = CChixelProperties::fs_Weight(Flags);
							if (Weight != LastProperties.f_Weight())
								SgrSequence.f_Weight(Weight);

							auto Underline = CChixelProperties::fs_Underline(Flags);
							if (Underline != LastProperties.f_Underline())
								SgrSequence.f_Underline(Underline);

							if (fg_IsSet(Flags, EChixelFlag::mc_Italic) != fg_IsSet(LastProperties.m_Flags, EChixelFlag::mc_Italic))
							{
								if (fg_IsSet(Flags, EChixelFlag::mc_Italic))
									SgrSequence.f_Italic();
								else
									SgrSequence.f_NotItalic();
							}

							if (fg_IsSet(Flags, EChixelFlag::mc_Strikeout) != fg_IsSet(LastProperties.m_Flags, EChixelFlag::mc_Strikeout))
							{
								if (fg_IsSet(Flags, EChixelFlag::mc_Strikeout))
									SgrSequence.f_Strikeout();
								else
									SgrSequence.f_NotStrikeout();
							}
						}

						if (ForegroundColor != LastProperties.m_ForegroundColor || fg_Exchange(bFirstForgroundColor, false))
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

						if (BackgroundColor != LastProperties.m_BackgroundColor || fg_Exchange(bFirstBackgroundColor, false))
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

						if (UnderlineColor != LastProperties.m_UnderlineColor || fg_Exchange(bFirstUnderlineColor, false))
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

					StringAppender.f_AddUnicodeChar(pBuffer->f_GetCharacter());

	#if DCommandLineSupportOverflow
					if (pBuffer->m_bHasOverflow)
					{
						if (NStr::CStr const *pOverflow = fp_FindOverflow(iYCoord * Width + iXCoord))
							StringAppender += *pOverflow;
					}
	#endif

					LastProperties.m_ForegroundColor = ForegroundColor;
					LastProperties.m_BackgroundColor = BackgroundColor;
					LastProperties.m_UnderlineColor = UnderlineColor;
					LastProperties.m_Flags = Flags;

					++pBuffer;
					++pPreviousBuffer;
				}
			}

			// Leave the terminal in the default state so output between frames does not inherit
			// the styling of the last changed cell
			if (iYCoordLast != TCLimitsInt<umint>::mc_Max)
				StringAppender += mp_AnsiEncoding.f_Default();

			if ((_Flags & ETerminalRenderFlag::mc_NoSynchronizedUpdate) == ETerminalRenderFlag::mc_None)
				StringAppender += mp_AnsiEncoding.f_SyncronizeOutputFinish();
		}

		return OutputString;
	}
}
