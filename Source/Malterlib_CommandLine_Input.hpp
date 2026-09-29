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

	inline bool CKeyEvent::f_IsBaseLayout(EKey _Key) const
	{
		return m_BaseLayoutKey == ch32(_Key);
	}

	inline bool CKeyEvent::f_IsBaseLayout(ch32 _Key) const
	{
		return m_BaseLayoutKey == _Key;
	}

	// m_HandlingReportID is between a terminal and what reads it, and is not streamed
	template <typename tf_CStream>
	void CKeyEvent::f_Stream(tf_CStream &_Stream)
	{
		_Stream % m_Text;
		_Stream % m_ScanCode;
		_Stream % m_BaseLayoutKey;
		_Stream % m_Modifiers;
		_Stream % m_EventType;
	}
}
