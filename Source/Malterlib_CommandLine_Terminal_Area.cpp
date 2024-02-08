// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <Mib/Core/Core>

#include "Malterlib_CommandLine_Terminal.h"

namespace NMib::NCommandLine
{
	namespace
	{
		// The default size is the int32 maximum, so a corner of an area placed anywhere but the
		// origin would wrap; it saturates at the edge instead
		int32 fg_SaturatingAdd(int32 _First, int32 _Second)
		{
			int64 Sum = int64(_First) + int64(_Second);
			if (Sum > TCLimitsInt<int32>::mc_Max)
				return TCLimitsInt<int32>::mc_Max;
			if (Sum < TCLimitsInt<int32>::mc_Min)
				return TCLimitsInt<int32>::mc_Min;

			return int32(Sum);
		}
	}

	CTerminalArea &CTerminalArea::operator += (CTerminalPoint const &_Right)
	{
		m_UpperLeft += _Right;
		return *this;
	}

	CTerminalArea &CTerminalArea::operator += (CTerminalSize const &_Right)
	{
		m_Size += _Right;
		return *this;
	}

	CTerminalArea CTerminalArea::operator + (CTerminalPoint const &_Right) const
	{
		CTerminalArea Return = *this;
		Return += _Right;
		return Return;
	}

	CTerminalArea CTerminalArea::operator + (CTerminalSize const &_Right) const
	{
		CTerminalArea Return = *this;
		Return += _Right;
		return Return;
	}

	bool CTerminalArea::operator == (CTerminalArea const &_Right) const
	{
		return f_Tuple() == _Right.f_Tuple();
	}

	COrdering_Strong CTerminalArea::operator <=> (CTerminalArea const &_Right) const
	{
		return f_Tuple() <=> _Right.f_Tuple();
	}

	CTerminalArea CTerminalArea::f_Intersection(CTerminalArea const &_Other) const
	{
		CTerminalPoint UpperLeft = m_UpperLeft;
		CTerminalPoint LowerRight = f_LowerRight();

		CTerminalPoint UpperLeftOther = _Other.m_UpperLeft;
		CTerminalPoint LowerRightOther = _Other.f_LowerRight();

		CTerminalPoint NewUpperLeft = {fg_Max(UpperLeft.m_CoordX, UpperLeftOther.m_CoordX), fg_Max(UpperLeft.m_CoordY, UpperLeftOther.m_CoordY)};
		CTerminalPoint NewLowerRight = {fg_Min(LowerRight.m_CoordX, LowerRightOther.m_CoordX), fg_Min(LowerRight.m_CoordY, LowerRightOther.m_CoordY)};

		CTerminalArea Return;
		Return.m_UpperLeft = NewUpperLeft;
		Return.m_Size = (NewLowerRight - NewUpperLeft).f_Size();

		return Return;
	}

	CTerminalArea CTerminalArea::f_Intersection(CTerminalSize const &_Other) const
	{
		return f_Intersection(CTerminalArea{{0,0}, _Other});
	}

	CTerminalArea CTerminalArea::f_Crop(CTerminalSize const &_Other) const
	{
		return f_Intersection(CTerminalArea{{0,0}, _Other});
	}

	CTerminalPoint CTerminalArea::f_UpperLeft() const
	{
		return m_UpperLeft;
	}

	CTerminalPoint CTerminalArea::f_UpperRight() const
	{
		return {fg_SaturatingAdd(m_UpperLeft.m_CoordX, m_Size.m_Width), m_UpperLeft.m_CoordY};
	}

	CTerminalPoint CTerminalArea::f_LowerLeft() const
	{
		return {m_UpperLeft.m_CoordX, fg_SaturatingAdd(m_UpperLeft.m_CoordY, m_Size.m_Height)};
	}

	CTerminalPoint CTerminalArea::f_LowerRight() const
	{
		return {fg_SaturatingAdd(m_UpperLeft.m_CoordX, m_Size.m_Width), fg_SaturatingAdd(m_UpperLeft.m_CoordY, m_Size.m_Height)};
	}

	CTerminalPoint CTerminalArea::f_Center() const
	{
		return m_UpperLeft + m_Size.f_Center();
	}

	CTerminalArea CTerminalArea::f_Centered(CTerminalSize const &_Size) const
	{
		return f_Centered(_Size.f_Area());
	}

	CTerminalArea CTerminalArea::f_Centered(CTerminalArea const &_Area) const
	{
		return {_Area.f_Center() - m_Size.f_Center(), m_Size};
	}

	bool CTerminalArea::f_Contains(CTerminalPoint const &_Point) const
	{
		if (_Point.m_CoordX < m_UpperLeft.m_CoordX)
			return false;

		if (_Point.m_CoordY < m_UpperLeft.m_CoordY)
			return false;

		auto LowerRight = f_LowerRight();

		if (_Point.m_CoordX >= LowerRight.m_CoordX)
			return false;

		if (_Point.m_CoordY >= LowerRight.m_CoordY)
			return false;

		return true;
	}

	bool CTerminalArea::f_Contains(CTerminalArea const &_Area) const
	{
		return f_Contains(_Area.m_UpperLeft) && f_Contains(_Area.f_LowerRight() + CTerminalPoint{-1, -1});
	}

	bool CTerminalArea::f_Intersects(CTerminalArea const &_Area) const
	{
		// Symmetric half-open interval overlap on both axes; corner containment misses crossing and containing areas
		auto LowerRight = f_LowerRight();
		auto OtherLowerRight = _Area.f_LowerRight();

		return m_UpperLeft.m_CoordX < OtherLowerRight.m_CoordX
			&& _Area.m_UpperLeft.m_CoordX < LowerRight.m_CoordX
			&& m_UpperLeft.m_CoordY < OtherLowerRight.m_CoordY
			&& _Area.m_UpperLeft.m_CoordY < LowerRight.m_CoordY
		;
	}

	bool CTerminalArea::f_IsValid()
	{
		return m_UpperLeft != CTerminalPoint{0, 0} || m_Size != CTerminalSize{TCLimitsInt<int32>::mc_Max, TCLimitsInt<int32>::mc_Max};
	}
}
