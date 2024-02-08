// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

namespace NMib::NCommandLine
{
	template <typename tf_CStr>
	void CTerminalSize::f_Format(tf_CStr &o_Str) const
	{
		o_Str += typename tf_CStr::CFormat("w:{} h:{}") << m_Width << m_Height;
	}
}
