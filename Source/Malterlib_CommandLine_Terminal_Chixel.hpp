// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

namespace NMib::NCommandLine
{
	template <typename tf_CStr>
	void CChixelProperties::f_Format(tf_CStr &o_Str) const
	{
		o_Str += typename tf_CStr::CFormat("Background: {} Foreground: {} UnderlineColor: {} Weight: {} Underline: {} Italic: {} Strikethrough: {}")
			<< m_BackgroundColor
			<< m_ForegroundColor
			<< m_UnderlineColor
			<< CChixelProperties::fs_WeightToString(f_Weight())
			<< CChixelProperties::fs_UnderlineToString(f_Underline())
			<< f_Italic()
			<< f_Strikeout()
		;
	}

	template <typename tf_CStr>
	void CChixel::f_Format(tf_CStr &o_Str) const
	{
		// Overflow codepoints live in the owning surface and are not part of the chixel
		NStr::CUStr Character;
		Character.f_AddChar(f_GetCharacter());
		o_Str += typename tf_CStr::CFormat("Charater: {} Background: {} Foreground: {} UnderlineColor: {} Weight: {} Underline: {} Italic: {} Strikethrough: {}")
			<< Character
			<< m_BackgroundColor
			<< m_ForegroundColor
			<< m_UnderlineColor
			<< CChixelProperties::fs_WeightToString(f_Weight())
			<< CChixelProperties::fs_UnderlineToString(f_Underline())
			<< f_Italic()
			<< f_Strikeout()
		;
	}

	template <typename tf_CStr>
	void CSetChixelProperties::f_Format(tf_CStr &o_Str) const
	{
		o_Str += typename tf_CStr::CFormat("Background: {} Foreground: {} UnderlineColor: {} Weight: {} Underline: {} Italic: {} Strikethrough: {}")
			<< m_BackgroundColor
			<< m_ForegroundColor
			<< m_UnderlineColor
			<< CChixelProperties::fs_WeightToString(f_Weight())
			<< CChixelProperties::fs_UnderlineToString(f_Underline())
			<< f_Italic()
			<< f_Strikeout()
		;
	}
}
