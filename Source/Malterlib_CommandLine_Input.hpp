// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

namespace NMib::NCommandLine
{
	inline bool CKeyEvent::f_Is(EKey _Key) const
	{
		return m_ScanCode == ch32(_Key);
	}

	inline bool CKeyEvent::f_Is(EKey _Key, EKeyModifier _Modifiers) const
	{
		return m_ScanCode == ch32(_Key) && m_Modifiers == _Modifiers;
	}
}
