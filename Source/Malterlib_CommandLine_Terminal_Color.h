// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

#include <Mib/Graphics/ColorA8R8G8B8>

namespace NMib::NCommandLine
{
	using CTerminalColor = NGraphics::CColorA8R8G8B8;

	constexpr CTerminalColor fg_ToTerminalColor(CAnsiEncodingParse::CDecodedColor const &_Color);
}

#include "Malterlib_CommandLine_Terminal_Color.hpp"
