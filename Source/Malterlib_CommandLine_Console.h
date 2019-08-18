// Copyright © 2015 Hansoft AB 
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

namespace NMib::NCommandLine
{
	void fg_MalterlibConOut(ch8 const *_pStr, mint _Len);
	void fg_MalterlibConOut(const NStr::CStrNonTracked &_Str);
	void fg_MalterlibConOut(NSys::EColor _Foreground,const NStr::CStrNonTracked &_Str);
	void fg_MalterlibConErrOut(const NStr::CStrNonTracked &_Str);
	void fg_MalterlibConErrOut(NSys::EColor _Foreground,const NStr::CStrNonTracked &_Str);

#	ifndef DMibEnableConsole
#		define DMibEnableConsole 1
#	endif

#	if DMibEnableConsole > 0
#		define DMibConOutRaw NMib::NCommandLine::fg_MalterlibConOut
#		define DMibConOut(_Format, _Args) NMib::NCommandLine::fg_MalterlibConOut((NMib::NStr::fg_GetStringFormat(_Format) << _Args).f_GetStr())
#		define DMibColorConOut(_Color, _Format, _Args) NMib::NCommandLine::fg_MalterlibConOut(_Color, (NMib::NStr::fg_GetStringFormat(_Format) << _Args).f_GetStr())
#		define DMibConErrOut(_Format, _Args) NMib::NCommandLine::fg_MalterlibConErrOut((NMib::NStr::fg_GetStringFormat(_Format) << _Args).f_GetStr())
#		define DMibConErrOutRaw NMib::NCommandLine::fg_MalterlibConErrOut
#		define DMibColorConErrOut(_Color, _Format, _Args) NMib::NCommandLine::fg_MalterlibConErrOut(_Color, (NMib::NStr::fg_GetStringFormat(_Format) << _Args).f_GetStr())

#		define DMibConOut2(...) NMib::NCommandLine::fg_MalterlibConOut((NMib::NStr::fg_Format<NMib::NStr::CStrNonTracked>(__VA_ARGS__)).f_GetStr())
#		define DMibColorConOut2(_Color, ...) NMib::NCommandLine::fg_MalterlibConOut(_Color, (NMib::NStr::fg_Format<NMib::NStr::CStrNonTracked>(__VA_ARGS__)).f_GetStr())
#		define DMibConErrOut2(...) NMib::NCommandLine::fg_MalterlibConErrOut((NMib::NStr::fg_Format<NMib::NStr::CStrNonTracked>(__VA_ARGS__)).f_GetStr())
#		define DMibColorConErrOut2(_Color, ...) NMib::NCommandLine::fg_MalterlibConErrOut(_Color, (NMib::NStr::fg_Format<NMib::NStr::CStrNonTracked>(__VA_ARGS__)).f_GetStr())
#	else
#		define DMibConOutRaw 1 ? (void)0 : NMib::NCommandLine::fg_MalterlibConOut
#		define DMibConErrOutRaw 1 ? (void)0 : NMib::NCommandLine::fg_MalterlibConErrOut
#		define DMibConOut(_Format, _Args) (void)0
#		define DMibColorConOut(_Color, _Format, _Args) (void)0
#		define DMibConErrOut(_Format, _Args) (void)0
#		define DMibColorConErrOut(_Color, _Format, _Args) (void)0

#		define DMibConOut2(...)  (void)0
#		define DMibColorConOut2(_Color, ...)  (void)0
#		define DMibConErrOut2(...)  (void)0
#		define DMibColorConErrOut2(_Color, ...)  (void)0
#	endif

#	ifndef DMibPNoShortCuts
#		define DConOut DMibConOut
#		define DColorConOut DMibConOut
#		define DConOutRaw DMibConOutRaw
#		define DConErrOut DMibConErrOut
#		define DColorConErrOut DMibConErrOut
#		define DConErrOutRaw DMibConErrOutRaw

#		define DConOut2 DMibConOut2
#		define DColorConOut2 DMibConOut2
#		define DConErrOut2 DMibConErrOut2
#		define DColorConErrOut2 DMibConErrOut2
#	endif

}

#ifndef DMibPNoShortCuts
	using namespace NMib::NCommandLine;
#endif
