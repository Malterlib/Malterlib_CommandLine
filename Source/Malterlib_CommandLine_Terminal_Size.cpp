// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <Mib/Core/Core>

#include "Malterlib_CommandLine_Terminal.h"

namespace NMib::NCommandLine
{
	CTerminalSize &CTerminalSize::operator += (CTerminalSize const &_Right)
	{
		m_Width += _Right.m_Width;
		m_Height += _Right.m_Height;
		return *this;
	}

	CTerminalSize CTerminalSize::operator + (CTerminalSize const &_Right) const
	{
		CTerminalSize Return = *this;
		Return += _Right;
		return Return;
	}

	CTerminalSize &CTerminalSize::operator -= (CTerminalSize const &_Right)
	{
		m_Width -= _Right.m_Width;
		m_Height -= _Right.m_Height;
		return *this;
	}

	CTerminalSize CTerminalSize::operator - (CTerminalSize const &_Right) const
	{
		CTerminalSize Return = *this;
		Return -= _Right;
		return Return;
	}

	CTerminalArea CTerminalSize::f_Area() const
	{
		return {{0, 0}, *this};
	}

	CTerminalPoint CTerminalSize::f_Center() const
	{
		return {m_Width / 2, m_Height / 2};
	}

	bool CTerminalSize::operator == (CTerminalSize const &_Right) const
	{
		return f_Tuple() == _Right.f_Tuple();
	}

	COrdering_Strong CTerminalSize::operator <=> (CTerminalSize const &_Right) const
	{
		return f_Tuple() <=> _Right.f_Tuple();
	}
}
