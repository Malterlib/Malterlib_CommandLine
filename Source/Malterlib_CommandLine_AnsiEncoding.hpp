// Copyright © 2018 Nonna Holding AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

namespace NMib::NCommandLine
{
	template <typename tf_CToWrap>
	NStr::CStr CAnsiEncoding::f_SyntaxColor(ESyntaxColor _Color, tf_CToWrap const &_ToWrap) const
	{
		return f_SyntaxColor(_Color) + NStr::CStr::fs_ToStr(_ToWrap) + f_Default();
	}

	template <typename tf_CType>
	NStr::CStr CAnsiEncoding::f_CreateSemiUniqueColor(tf_CType const &_Data) const
	{
		NStr::CStr DataStr = NStr::CStr::fs_ToStr(_Data);
		return f_CreateSemiUniqueColor(DataStr);
	}

	template <typename tf_CType>
	NStr::CStr CAnsiEncoding::f_ColorSemiUnique(tf_CType const &_Data) const
	{
		return NStr::CStr::CFormat("{}{}{}") << f_CreateSemiUniqueColor(_Data) << _Data << f_Default();
	}

	template <typename tf_CStr>
	void CAnsiEncoding::CLine::f_Format(tf_CStr &o_String) const
	{
		o_String += typename tf_CStr::CFormat("{}: {}") << m_Width << m_String;
	}
}
