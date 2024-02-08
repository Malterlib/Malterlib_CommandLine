// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

namespace NMib::NCommandLine
{
	struct CTerminalPoint
	{
		CTerminalPoint &operator += (CTerminalPoint const &_Right);
		CTerminalPoint &operator += (CTerminalSize const &_Right);
		CTerminalPoint &operator -= (CTerminalPoint const &_Right);
		CTerminalPoint &operator -= (CTerminalSize const &_Right);
		CTerminalPoint operator + (CTerminalPoint const &_Right) const;
		CTerminalPoint operator + (CTerminalSize const &_Right) const;
		CTerminalPoint operator - (CTerminalPoint const &_Right) const;
		CTerminalPoint operator - (CTerminalSize const &_Right) const;

		CTerminalSize f_Size() const;

		auto f_Tuple() const;
		bool operator == (CTerminalPoint const &_Right) const;
		COrdering_Strong operator <=> (CTerminalPoint const &_Right) const;

		template <typename tf_CStr>
		void f_Format(tf_CStr &o_Str) const;

		int32 m_CoordX = 0;
		int32 m_CoordY = 0;
	};
}

#include "Malterlib_CommandLine_Terminal_Point.hpp"
