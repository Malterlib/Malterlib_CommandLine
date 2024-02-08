// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

namespace NMib::NCommandLine
{
	struct CTerminalArea;
	struct CTerminalPoint;
	
	struct CTerminalSize
	{
		CTerminalSize &operator += (CTerminalSize const &_Right);
		CTerminalSize operator + (CTerminalSize const &_Right) const;
		CTerminalSize &operator -= (CTerminalSize const &_Right);
		CTerminalSize operator - (CTerminalSize const &_Right) const;

		CTerminalArea f_Area() const;
		CTerminalPoint f_Center() const;

		auto f_Tuple() const;
		bool operator == (CTerminalSize const &_Right) const;
		COrdering_Strong operator <=> (CTerminalSize const &_Right) const;

		template <typename tf_CStr>
		void f_Format(tf_CStr &o_Str) const;

		int32 m_Width = TCLimitsInt<int32>::mc_Max;
		int32 m_Height = TCLimitsInt<int32>::mc_Max;
	};
}

#include "Malterlib_CommandLine_Terminal_Size.hpp"
