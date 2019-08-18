// Copyright © 2015 Hansoft AB 
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#include "Malterlib_CommandLine.h"

namespace NMib::NCommandLine
{
	void fg_MalterlibConOut(ch8 const *_pStr, mint _Len)
	{
		NSys::fg_ConsoleOutput(_pStr, _Len);
	}

	void fg_MalterlibConOut(const NStr::CStrNonTracked &_Str)
	{
		NSys::fg_ConsoleOutput(_Str);
	}
	void fg_MalterlibConOut(NSys::EColor _Foreground, const NStr::CStrNonTracked &_Str)
	{
		NSys::fg_ConsoleOutput(_Foreground, _Str);
	}

	void fg_MalterlibConErrOut(const NStr::CStrNonTracked &_Str)
	{
		NSys::fg_ConsoleErrorOutput(_Str);
	}
	void fg_MalterlibConErrOut(NSys::EColor _Foreground, const NStr::CStrNonTracked &_Str)
	{
		NSys::fg_ConsoleErrorOutput(_Foreground, _Str);
	}
}
