// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <Mib/Core/Core>
#include <Mib/CommandLine/AnsiEncodingParse>

#include "Malterlib_CommandLine_Terminal.h"
#include "Malterlib_CommandLine_Terminal_Surface_BoxMerges.hpp"
#include "Malterlib_CommandLine_Terminal_Surface_BoxTable.hpp"

namespace NMib::NCommandLine
{
	using namespace NStr;

#if DCalculateBoxMerges
	static constinit NStorage::TCAggregate<CBoxMergesSorted> g_BoxMerges = {DAggregateInit};
#else
	static constexpr CBoxMergesSorted gc_BoxMerges;
#endif

	void CTerminalSurface::f_Box(CTerminalArea const &_Area, CRenderBoxOptions const &_Options)
	{
		CTerminalArea Area = _Area.f_Intersection(mp_Size);
		if (Area.m_Size.m_Height <= 0 || Area.m_Size.m_Width <= 0)
			return;

#if DCalculateBoxMerges
		auto const &BoxMerges = *g_BoxMerges;
#else
		auto const &BoxMerges = gc_BoxMerges;
#endif

		auto &OriginalUpperLeft = _Area.m_UpperLeft;
		auto OriginalLowerRight = _Area.f_LowerRight();

		int32 OriginalTop = OriginalUpperLeft.m_CoordY;
		int32 OriginalBottom = OriginalLowerRight.m_CoordY - 1;
		int32 OriginalLeft = OriginalUpperLeft.m_CoordX;
		int32 OriginalRight = OriginalLowerRight.m_CoordX - 1;
		
		auto &UpperLeft = Area.m_UpperLeft;
		auto LowerRight = Area.f_LowerRight();
		auto pBuffer = mp_Buffer.f_GetArray();

		auto const &BoxDrawingCharacters = gc_TerminalBoxCharacters[(int)_Options.m_Type];

		auto BoxCoverage = _Options.m_BoxCoverage;
		bool bMergeBoxes = _Options.m_bMergeBoxes;

		int32 SurfaceWidth = mp_Size.m_Width;
		int32 Top = UpperLeft.m_CoordY;
		int32 Bottom = LowerRight.m_CoordY - 1;
		int32 Left = UpperLeft.m_CoordX;
		int32 Right = LowerRight.m_CoordX - 1;

		auto fWriteCharacter = [&](CChixel &_Destination, ch32 _NewCharacter)
			{
				if (!bMergeBoxes)
				{
					_Destination.f_SetCharacterAndEndpointType(_NewCharacter);
					return;
				}

				auto OldCharacter = _Destination.f_GetCharacterAndEndpointType();
				if (auto Combined = BoxMerges.f_GetCombined(OldCharacter, _NewCharacter))
					_Destination.f_SetCharacterAndEndpointTypeRaw(Combined);
				else
					_Destination.f_SetCharacterAndEndpointTypeRaw(_NewCharacter);
			}
		;

		for (int32 CoordX = fg_Min(Left + 1, fg_Max(OriginalLeft + 1, 0)); CoordX <= Right && CoordX < OriginalRight; ++CoordX)
		{
			if ((BoxCoverage & ETerminalBoxCoverage::mc_Top) != ETerminalBoxCoverage::mc_None && Top == OriginalTop)
				fWriteCharacter(pBuffer[Top * SurfaceWidth + CoordX], BoxDrawingCharacters.m_HorizontalTop);
			if ((BoxCoverage & ETerminalBoxCoverage::mc_Bottom) != ETerminalBoxCoverage::mc_None && OriginalBottom == Bottom)
				fWriteCharacter(pBuffer[Bottom * SurfaceWidth + CoordX], BoxDrawingCharacters.m_HorizontalBottom);
		}

		for (int32 CoordY = fg_Min(Top + 1, fg_Max(OriginalTop + 1, 0)); CoordY <= Bottom && CoordY < OriginalBottom; ++CoordY)
		{
			if ((BoxCoverage & ETerminalBoxCoverage::mc_Left) != ETerminalBoxCoverage::mc_None && Left == OriginalLeft)
				fWriteCharacter(pBuffer[CoordY * SurfaceWidth + Left], BoxDrawingCharacters.m_VerticalLeft);
			if ((BoxCoverage & ETerminalBoxCoverage::mc_Right) != ETerminalBoxCoverage::mc_None && Right == OriginalRight)
				fWriteCharacter(pBuffer[CoordY * SurfaceWidth + Right],  BoxDrawingCharacters.m_VerticalRight);
		}

		if (Top == OriginalTop)
		{
			if (Left == OriginalLeft)
			{
				switch (BoxCoverage & (ETerminalBoxCoverage::mc_Top | ETerminalBoxCoverage::mc_Left))
				{
				case ETerminalBoxCoverage::mc_Top | ETerminalBoxCoverage::mc_Left: fWriteCharacter(pBuffer[Top * SurfaceWidth + Left], BoxDrawingCharacters.m_TopLeft); break;
				case ETerminalBoxCoverage::mc_Top: fWriteCharacter(pBuffer[Top * SurfaceWidth + Left], BoxDrawingCharacters.m_HorizontalTopLeftOpen); break;
				case ETerminalBoxCoverage::mc_Left: fWriteCharacter(pBuffer[Top * SurfaceWidth + Left], BoxDrawingCharacters.m_VerticalTopLeftOpen); break;
				default: break;
				}
			}
			if (Right == OriginalRight)
			{
				switch (BoxCoverage & (ETerminalBoxCoverage::mc_Top | ETerminalBoxCoverage::mc_Right))
				{
				case ETerminalBoxCoverage::mc_Top | ETerminalBoxCoverage::mc_Right: fWriteCharacter(pBuffer[Top * SurfaceWidth + Right], BoxDrawingCharacters.m_TopRight); break;
				case ETerminalBoxCoverage::mc_Top: fWriteCharacter(pBuffer[Top * SurfaceWidth + Right], BoxDrawingCharacters.m_HorizontalTopRightOpen); break;
				case ETerminalBoxCoverage::mc_Right: fWriteCharacter(pBuffer[Top * SurfaceWidth + Right], BoxDrawingCharacters.m_VerticalTopRightOpen); break;
				default: break;
				}
			}
		}

		if (Bottom == OriginalBottom)
		{
			if (Left == OriginalLeft)
			{
				switch (BoxCoverage & (ETerminalBoxCoverage::mc_Bottom | ETerminalBoxCoverage::mc_Left))
				{
				case ETerminalBoxCoverage::mc_Bottom | ETerminalBoxCoverage::mc_Left: fWriteCharacter(pBuffer[Bottom * SurfaceWidth + Left], BoxDrawingCharacters.m_BottomLeft); break;
				case ETerminalBoxCoverage::mc_Bottom: fWriteCharacter(pBuffer[Bottom * SurfaceWidth + Left], BoxDrawingCharacters.m_HorizontalBottomLeftOpen); break;
				case ETerminalBoxCoverage::mc_Left: fWriteCharacter(pBuffer[Bottom * SurfaceWidth + Left], BoxDrawingCharacters.m_VerticalBottomLeftOpen); break;
				default: break;
				}
			}
			if (Right == OriginalRight)
			{
				switch (BoxCoverage & (ETerminalBoxCoverage::mc_Bottom | ETerminalBoxCoverage::mc_Right))
				{
				case ETerminalBoxCoverage::mc_Bottom | ETerminalBoxCoverage::mc_Right: fWriteCharacter(pBuffer[Bottom * SurfaceWidth + Right], BoxDrawingCharacters.m_BottomRight); break;
				case ETerminalBoxCoverage::mc_Bottom: fWriteCharacter(pBuffer[Bottom * SurfaceWidth + Right], BoxDrawingCharacters.m_HorizontalBottomRightOpen); break;
				case ETerminalBoxCoverage::mc_Right: fWriteCharacter(pBuffer[Bottom * SurfaceWidth + Right], BoxDrawingCharacters.m_VerticalBottomRightOpen); break;
				default: break;
				}
			}
		}
	}
}
