// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <Mib/Core/Core>
#include <Mib/CommandLine/AnsiEncodingParse>

#include "Malterlib_CommandLine_Terminal.h"

namespace NMib::NCommandLine
{
	using namespace NStr;

	void CTerminalSurface::f_Blit(CTerminalSurface const &_Source, CTerminalArea const &_SourceArea, CTerminalPoint const &_Destination)
	{
		// A surface blitted onto itself reads cells the copy has already written once the areas
		// overlap; a snapshot keeps every read at the original
		if (&_Source == this)
		{
			CTerminalSurface Snapshot(_Source);
			f_Blit(Snapshot, _SourceArea, _Destination);
			return;
		}

		auto SourceArea = _SourceArea.f_Intersection(_Source.f_Size());

		// Clipping moves the source's upper left, and the destination moves with it, so every
		// source cell keeps the destination it was given
		auto Destination = _Destination + (SourceArea.m_UpperLeft - _SourceArea.m_UpperLeft);
		auto DestinationArea = CTerminalArea{Destination, SourceArea.m_Size};

		CTerminalArea Area = DestinationArea.f_Intersection(mp_Size);

		if (Area.m_Size.m_Height <= 0 || Area.m_Size.m_Width <= 0)
			return;
		
		auto &UpperLeft = Area.m_UpperLeft;
		auto LowerRight = Area.f_LowerRight();
		auto pBuffer = mp_Buffer.f_GetArray();
		auto pSourceBuffer = _Source.mp_Buffer.f_GetArray();

		auto SourceOffset = SourceArea.m_UpperLeft - Destination;

		int32 SurfaceWidth = mp_Size.m_Width;
		int32 SourceSurfaceWidth = _Source.mp_Size.m_Width;

#if DCommandLineSupportOverflow
		// Cells referencing overflow strings need their entries carried over; a source without any
		// overflow keeps the copy loop free of per-cell branches
		if (!_Source.mp_OverflowStrings.f_IsEmpty())
		{
			for (int32 CoordY = UpperLeft.m_CoordY; CoordY < LowerRight.m_CoordY; ++CoordY)
			{
				for (int32 CoordX = UpperLeft.m_CoordX; CoordX < LowerRight.m_CoordX; ++CoordX)
				{
					umint iCell = umint(CoordY * SurfaceWidth + CoordX);
					umint iSourceCell = umint((CoordY + SourceOffset.m_CoordY) * SourceSurfaceWidth + (CoordX + SourceOffset.m_CoordX));

					auto &Chixel = pBuffer[iCell];
					auto &SourceChixel = pSourceBuffer[iSourceCell];

					Chixel = SourceChixel;
					if (SourceChixel.m_bHasOverflow)
					{
						if (NStr::CStr const *pOverflow = _Source.fp_FindOverflow(iSourceCell))
							mp_OverflowStrings[iCell] = *pOverflow;
					}
				}
			}

			return;
		}
#endif

		for (int32 CoordY = UpperLeft.m_CoordY; CoordY < LowerRight.m_CoordY; ++CoordY)
		{
			for (int32 CoordX = UpperLeft.m_CoordX; CoordX < LowerRight.m_CoordX; ++CoordX)
			{
				auto &Chixel = pBuffer[CoordY * SurfaceWidth + CoordX];
				auto &SourceChixel = pSourceBuffer[(CoordY + SourceOffset.m_CoordY) * SourceSurfaceWidth + (CoordX + SourceOffset.m_CoordX)];

				Chixel = SourceChixel;
			}
		}
	}

	void CTerminalSurface::f_BlitMerge(CTerminalSurface const &_Source, CTerminalArea const &_SourceArea, CTerminalPoint const &_Destination)
	{
		// As for f_Blit: a merge from the surface itself reads from a snapshot
		if (&_Source == this)
		{
			CTerminalSurface Snapshot(_Source);
			f_BlitMerge(Snapshot, _SourceArea, _Destination);
			return;
		}

		auto SourceArea = _SourceArea.f_Intersection(_Source.f_Size());

		// Clipping moves the source's upper left, and the destination moves with it, so every
		// source cell keeps the destination it was given
		auto Destination = _Destination + (SourceArea.m_UpperLeft - _SourceArea.m_UpperLeft);
		auto DestinationArea = CTerminalArea{Destination, SourceArea.m_Size};

		CTerminalArea Area = DestinationArea.f_Intersection(mp_Size);

		if (Area.m_Size.m_Height <= 0 || Area.m_Size.m_Width <= 0)
			return;

		auto &UpperLeft = Area.m_UpperLeft;
		auto LowerRight = Area.f_LowerRight();
		auto pBuffer = mp_Buffer.f_GetArray();
		auto pSourceBuffer = _Source.mp_Buffer.f_GetArray();

		auto SourceOffset = SourceArea.m_UpperLeft - Destination;

		int32 SurfaceWidth = mp_Size.m_Width;
		int32 SourceSurfaceWidth = _Source.mp_Size.m_Width;

		for (int32 CoordY = UpperLeft.m_CoordY; CoordY < LowerRight.m_CoordY; ++CoordY)
		{
			for (int32 CoordX = UpperLeft.m_CoordX; CoordX < LowerRight.m_CoordX; ++CoordX)
			{
				auto &Chixel = pBuffer[CoordY * SurfaceWidth + CoordX];
				auto &SourceChixel = pSourceBuffer[(CoordY + SourceOffset.m_CoordY) * SourceSurfaceWidth + (CoordX + SourceOffset.m_CoordX)];

				bool bSourceHasGlyph = SourceChixel.f_GetCharacter() != ch32(' ')
					|| SourceChixel.m_ForegroundColor.f_Alpha() != 0
					|| SourceChixel.m_BackgroundColor.f_Alpha() != 0
				;

				if (!bSourceHasGlyph)
				{
					if (SourceChixel.m_UnderlineColor.f_Alpha() != 0)
						Chixel.m_UnderlineColor = SourceChixel.m_UnderlineColor;

					continue;
				}

				auto ForegroundColor = Chixel.m_ForegroundColor;
				auto BackgroundColor = Chixel.m_BackgroundColor;
				auto UnderlineColor = Chixel.m_UnderlineColor;

				Chixel = SourceChixel;

#if DCommandLineSupportOverflow
				if (SourceChixel.m_bHasOverflow)
				{
					umint iSourceCell = umint((CoordY + SourceOffset.m_CoordY) * SourceSurfaceWidth + (CoordX + SourceOffset.m_CoordX));
					if (NStr::CStr const *pOverflow = _Source.fp_FindOverflow(iSourceCell))
						mp_OverflowStrings[umint(CoordY * SurfaceWidth + CoordX)] = *pOverflow;
				}
#endif

				if (SourceChixel.m_ForegroundColor.f_Alpha() == 0)
					Chixel.m_ForegroundColor = ForegroundColor;

				if (SourceChixel.m_BackgroundColor.f_Alpha() == 0)
					Chixel.m_BackgroundColor = BackgroundColor;

				if (SourceChixel.m_UnderlineColor.f_Alpha() == 0)
					Chixel.m_UnderlineColor = UnderlineColor;
			}
		}
	}
}
