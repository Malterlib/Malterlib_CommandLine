// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

namespace NMib::NCommandLine
{
	constexpr CTerminalColor fg_ToTerminalColor(CAnsiEncodingParse::CDecodedColor const &_Color)
	{
		// A disabled color is a return to the default color, which is the transparent color here
		if (!_Color.m_bEnabled)
			return CTerminalColor::fs_FromRGBA(0, 0, 0, 0);

		return CTerminalColor::fs_FromRGBA(_Color.m_Red, _Color.m_Green, _Color.m_Blue);
	}
}
