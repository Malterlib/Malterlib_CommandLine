// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <Mib/Core/Core>

#include "Malterlib_CommandLine_Terminal.h"

namespace NMib::NCommandLine
{
	CTerminalPoint &CTerminalPoint::operator += (CTerminalPoint const &_Right)
	{
		m_CoordX += _Right.m_CoordX;
		m_CoordY += _Right.m_CoordY;
		return *this;
	}

	CTerminalPoint CTerminalPoint::operator + (CTerminalPoint const &_Right) const
	{
		CTerminalPoint Return = *this;
		Return += _Right;
		return Return;
	}

	CTerminalPoint CTerminalPoint::operator + (CTerminalSize const &_Right) const
	{
		CTerminalPoint Return = *this;
		Return += _Right;
		return Return;
	}

	CTerminalPoint &CTerminalPoint::operator += (CTerminalSize const &_Right)
	{
		m_CoordX += _Right.m_Width;
		m_CoordY += _Right.m_Height;
		return *this;
	}

	CTerminalPoint &CTerminalPoint::operator -= (CTerminalPoint const &_Right)
	{
		m_CoordX -= _Right.m_CoordX;
		m_CoordY -= _Right.m_CoordY;
		return *this;
	}

	CTerminalPoint &CTerminalPoint::operator -= (CTerminalSize const &_Right)
	{
		m_CoordX -= _Right.m_Width;
		m_CoordY -= _Right.m_Height;
		return *this;
	}

	CTerminalPoint CTerminalPoint::operator - (CTerminalPoint const &_Right) const
	{
		CTerminalPoint Return = *this;
		Return -= _Right;
		return Return;
	}

	CTerminalPoint CTerminalPoint::operator - (CTerminalSize const &_Right) const
	{
		CTerminalPoint Return = *this;
		Return -= _Right;
		return Return;
	}

	bool CTerminalPoint::operator == (CTerminalPoint const &_Right) const
	{
		return f_Tuple() == _Right.f_Tuple();
	}

	COrdering_Strong CTerminalPoint::operator <=> (CTerminalPoint const &_Right) const
	{
		return f_Tuple() <=> _Right.f_Tuple();
	}

	CTerminalSize CTerminalPoint::f_Size() const
	{
		return {m_CoordX, m_CoordY};
	}
}
