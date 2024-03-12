// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "Malterlib_CommandLine_Input.h"

namespace NMib::NCommandLine
{
	NStr::CStr CKeyEvent::fs_ModifiersToString(EKeyModifier _Modifiers)
	{
		NStr::CStr ReturnString;

		if (fg_IsSet(_Modifiers, EKeyModifier::mc_Shift))
			fg_AddStrSep(ReturnString, "Shift", ", ");
		if (fg_IsSet(_Modifiers, EKeyModifier::mc_Alt))
			fg_AddStrSep(ReturnString, "Alt", ", ");
		if (fg_IsSet(_Modifiers, EKeyModifier::mc_Ctrl))
			fg_AddStrSep(ReturnString, "Ctrl", ", ");
		if (fg_IsSet(_Modifiers, EKeyModifier::mc_Super))
			fg_AddStrSep(ReturnString, "Super", ", ");
		if (fg_IsSet(_Modifiers, EKeyModifier::mc_Hyper))
			fg_AddStrSep(ReturnString, "Hyper", ", ");
		if (fg_IsSet(_Modifiers, EKeyModifier::mc_Meta))
			fg_AddStrSep(ReturnString, "Meta", ", ");
		if (fg_IsSet(_Modifiers, EKeyModifier::mc_CapsLock))
			fg_AddStrSep(ReturnString, "CapsLock", ", ");
		if (fg_IsSet(_Modifiers, EKeyModifier::mc_NumLock))
			fg_AddStrSep(ReturnString, "NumLock", ", ");

		return ReturnString;
	}
}
