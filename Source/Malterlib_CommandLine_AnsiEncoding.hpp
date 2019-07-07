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
}
