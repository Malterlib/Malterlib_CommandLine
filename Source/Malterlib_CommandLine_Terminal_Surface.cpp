// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <Mib/Core/Core>
#include <Mib/CommandLine/AnsiEncodingParse>

#include "Malterlib_CommandLine_Terminal.h"

namespace NMib::NCommandLine
{
	using namespace NStr;

	CTerminalSurface::CTerminalSurface(CAnsiEncoding const &_AnsiEncoding, CTerminalSize const &_Size, CTerminalSurfaceClearProperties const &_ClearProperties)
		: mp_Size(_Size)
		, mp_AnsiEncoding(_AnsiEncoding)
	{
		DMibRequire(mp_Size.m_Width > 0);
		DMibRequire(mp_Size.m_Height > 0);

		// Cells are indexed in int32 arithmetic throughout, so the count has to fit it. The product
		// is formed wider than that, or a size past it would wrap to a small allocation
		uint64 nCells = uint64(mp_Size.m_Width) * uint64(mp_Size.m_Height);
		DMibRequire(nCells <= uint64(TCLimitsInt<int32>::mc_Max));
		mp_Buffer.f_SetLen(umint(nCells));
		f_Clear(_ClearProperties);
	}

	CTerminalSize const &CTerminalSurface::f_Size() const
	{
		return mp_Size;
	}

	CTerminalArea CTerminalSurface::f_Area() const
	{
		return {CTerminalPoint{0, 0}, mp_Size};
	}

	CChixel const &CTerminalSurface::f_GetChixel(CTerminalPoint const &_Point) const
	{
		DMibRequire(_Point.m_CoordX >= 0 && _Point.m_CoordX < mp_Size.m_Width);
		DMibRequire(_Point.m_CoordY >= 0 && _Point.m_CoordY < mp_Size.m_Height);

		return mp_Buffer[umint(_Point.m_CoordY) * umint(mp_Size.m_Width) + umint(_Point.m_CoordX)];
	}

#if DCommandLineSupportOverflow
	NStr::CStr const *CTerminalSurface::f_GetOverflow(CTerminalPoint const &_Point) const
	{
		if (!f_GetChixel(_Point).m_bHasOverflow)
			return nullptr;

		return fp_FindOverflow(umint(_Point.m_CoordY) * umint(mp_Size.m_Width) + umint(_Point.m_CoordX));
	}

	NStr::CStr const *CTerminalSurface::fp_FindOverflow(umint _iCell) const
	{
		return mp_OverflowStrings.f_FindEqual(_iCell);
	}
#endif
}
