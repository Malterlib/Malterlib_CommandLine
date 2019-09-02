// Copyright © 2018 Nonna Holding AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

namespace NMib::NCommandLine
{
	inline auto CAnsiEncodingParse::CDecodedColor::f_Tuple() const
	{
		return NStorage::fg_TupleReferences(m_Red, m_Green, m_Blue, m_bEnabled);
	}

	inline bool CAnsiEncodingParse::CBold::operator == (CBold const &_Other) const
	{
		return m_bEnabled == _Other.m_bEnabled;
	}

	inline bool CAnsiEncodingParse::CItalic::operator == (CItalic const &_Other) const
	{
		return m_bEnabled == _Other.m_bEnabled;
	}

	inline bool CAnsiEncodingParse::CReset::operator == (CReset const &_Other) const
	{
		return true;
	}
}
