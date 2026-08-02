// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

#include <Mib/Function/Function>

#include "Malterlib_CommandLine_Input.h"

namespace NMib::NCommandLine
{
	struct CAnsiEncodingParseInputOptions
	{
		NFunction::TCFunctionMovable<void (CKeyEvent &&_KeyEvent)> m_fOnKeyEvent;
		NFunction::TCFunctionMovable<void (CTerminalMouseEvent &&_MouseEvent)> m_fOnMouseEvent;
	};

	// Decodes raw terminal input (legacy escape sequences, the kitty comprehensive keyboard protocol
	// and SGR mouse reports) into key and mouse events. Incomplete trailing sequences are buffered
	// until the next f_AddInput call; when f_HasBufferedInput reports pending input and no
	// continuation arrives, the caller should call f_Flush after a short timeout so a buffered
	// escape is delivered as the Escape key.
	struct CAnsiEncodingParseInput
	{
		CAnsiEncodingParseInput(CAnsiEncodingParseInputOptions &&_Options);

		void f_AddInput(NStr::CStr const &_Input);

		bool f_HasBufferedInput() const;
		void f_Flush();

	private:
		void fp_EmitKey(CKeyEvent &&_KeyEvent);
		void fp_EmitControl(ch8 _Char);
		bool fp_ParseCsi(ch8 const *&_pParse, ch8 const *_pEnd);
		void fp_DispatchCsi(NStr::CStrPtr const &_Params, char _Final);
		void fp_DispatchCsiMouse(NStr::CStrPtr const &_Params, char _Final);

		CAnsiEncodingParseInputOptions mp_Options;
		NStr::CStr mp_InputBuffer;
	};
}

#ifndef DMibPNoShortCuts
	using namespace NMib::NCommandLine;
#endif
