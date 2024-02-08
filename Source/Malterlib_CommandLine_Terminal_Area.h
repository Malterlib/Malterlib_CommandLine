// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

namespace NMib::NCommandLine
{
	struct CTerminalArea
	{
		CTerminalArea &operator += (CTerminalPoint const &_Right);
		CTerminalArea &operator += (CTerminalSize const &_Right);
		CTerminalArea operator + (CTerminalPoint const &_Right) const;
		CTerminalArea operator + (CTerminalSize const &_Right) const;

		auto f_Tuple() const;
		bool operator == (CTerminalArea const &_Right) const;
		COrdering_Strong operator <=> (CTerminalArea const &_Right) const;

		template <typename tf_CStr>
		void f_Format(tf_CStr &o_Str) const;

		CTerminalArea f_Intersection(CTerminalArea const &_Other) const;
		CTerminalArea f_Intersection(CTerminalSize const &_Other) const;
		CTerminalArea f_Crop(CTerminalSize const &_Other) const;
		CTerminalPoint f_UpperLeft() const;
		CTerminalPoint f_UpperRight() const;
		CTerminalPoint f_LowerLeft() const;
		CTerminalPoint f_LowerRight() const;

		CTerminalPoint f_Center() const;
		CTerminalArea f_Centered(CTerminalSize const &_Size) const;
		CTerminalArea f_Centered(CTerminalArea const &_Area) const;

		bool f_Contains(CTerminalPoint const &_Point) const;
		bool f_Contains(CTerminalArea const &_Area) const;
		bool f_Intersects(CTerminalArea const &_Area) const;

		bool f_IsValid();

		CTerminalPoint m_UpperLeft;
		CTerminalSize m_Size;
	};
}

#include "Malterlib_CommandLine_Terminal_Area.hpp"
