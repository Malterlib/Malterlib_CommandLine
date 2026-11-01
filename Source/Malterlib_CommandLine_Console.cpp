// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "Malterlib_CommandLine.h"

namespace NMib::NCommandLine
{
	void fg_MalterlibConOut(ch8 const *_pStr, umint _Len)
	{
		NSys::fg_ConsoleOutput(_pStr, _Len);
	}

	void fg_MalterlibConOut(NStr::CStrNonTracked const &_Str)
	{
		NSys::fg_ConsoleOutput(_Str);
	}

	void fg_MalterlibConErrOut(NStr::CStrNonTracked const &_Str)
	{
		NSys::fg_ConsoleErrorOutput(_Str);
	}
}
