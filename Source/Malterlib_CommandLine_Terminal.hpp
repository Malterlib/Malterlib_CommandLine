// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

namespace NMib::NCommandLine
{
	inline auto CTerminalSize::f_Tuple() const
	{
		return NStorage::fg_TupleReferences(m_Width, m_Height);
	}

	inline auto CTerminalPoint::f_Tuple() const
	{
		return NStorage::fg_TupleReferences(m_CoordX, m_CoordY);
	}

	inline auto CTerminalArea::f_Tuple() const
	{
		return NStorage::fg_TupleReferences(m_UpperLeft, m_Size);
	}
}
