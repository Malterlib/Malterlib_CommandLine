// Copyright © 2015 Hansoft AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

namespace NMib::NCommandLine
{
	void fg_MalterlibConOut(ch8 const *_pStr, umint _Len);
	void fg_MalterlibConOut(const NStr::CStrNonTracked &_Str);
	void fg_MalterlibConErrOut(const NStr::CStrNonTracked &_Str);

#	ifndef DMibEnableConsole
#		define DMibEnableConsole 1
#	endif

#	if DMibEnableConsole > 0
#		define DMibConOutRaw(...) NMib::NCommandLine::fg_MalterlibConOut(__VA_ARGS__)
#		define DMibConErrOutRaw(...) NMib::NCommandLine::fg_MalterlibConErrOut(__VA_ARGS__)

#		define DMibConOut(...) NMib::NCommandLine::fg_MalterlibConOut((NMib::NStr::fg_Format<NMib::NStr::CStrNonTracked>(__VA_ARGS__)).f_GetStr())
#		define DMibColorConOut(_Color, ...) NMib::NCommandLine::fg_MalterlibConOut(_Color, (NMib::NStr::fg_Format<NMib::NStr::CStrNonTracked>(__VA_ARGS__)).f_GetStr())
#		define DMibConErrOut(...) NMib::NCommandLine::fg_MalterlibConErrOut((NMib::NStr::fg_Format<NMib::NStr::CStrNonTracked>(__VA_ARGS__)).f_GetStr())
#		define DMibColorConErrOut(_Color, ...) NMib::NCommandLine::fg_MalterlibConErrOut(_Color, (NMib::NStr::fg_Format<NMib::NStr::CStrNonTracked>(__VA_ARGS__)).f_GetStr())
#	else
#		define DMibConOutRaw(...) (void)0
#		define DMibConErrOutRaw(...) (void)0

#		define DMibConOut(...) (void)0
#		define DMibColorConOut(_Color, ...) (void)0
#		define DMibConErrOut(...) (void)0
#		define DMibColorConErrOut(_Color, ...) (void)0
#	endif

#	ifndef DMibPNoShortCuts
#		define DConOutRaw DMibConOutRaw
#		define DConErrOutRaw DMibConErrOutRaw

#		define DConOut DMibConOut
#		define DColorConOut DMibColorConOut
#		define DConErrOut DMibConErrOut
#		define DColorConErrOut DMibColorConErrOut
#	endif

}

#ifndef DMibPNoShortCuts
	using namespace NMib::NCommandLine;
#endif
