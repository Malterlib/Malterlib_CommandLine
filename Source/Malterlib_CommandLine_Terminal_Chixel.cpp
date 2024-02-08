// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <Mib/Core/Core>

#include "Malterlib_CommandLine_Terminal.h"

namespace NMib::NCommandLine
{
	void CChixelProperties::f_ChangeFlags(EChixelFlag _Remove, EChixelFlag _Add)
	{
		m_Flags = (m_Flags & (~_Remove)) | _Add;
	}

	void CSetChixelProperties::f_ChangeFlags(EChixelFlag _Remove, EChixelFlag _Add)
	{
		if (m_ChixelFlags)
			m_ChixelFlags = (*m_ChixelFlags & ~_Remove) | _Add;
		else
			m_ChixelFlags = _Add;
	}

	CAnsiEncoding::EWeight CSetChixelProperties::f_Weight() const
	{
		if (m_ChixelFlags)
			return CAnsiEncoding::EWeight(((*m_ChixelFlags) & EChixelFlag::mc_WeightMask) >> int(EChixelFlag::mc_WeightOffset));
		else
			return CAnsiEncoding::EWeight::mc_Normal;
	}

	CAnsiEncoding::EUnderline CSetChixelProperties::f_Underline() const
	{
		if (m_ChixelFlags)
			return CAnsiEncoding::EUnderline(((*m_ChixelFlags) & EChixelFlag::mc_UnderlineMask) >> int(EChixelFlag::mc_UnderlineOffset));
		else
			return CAnsiEncoding::EUnderline::mc_None;
	}

	bool CSetChixelProperties::f_Italic() const
	{
		if (m_ChixelFlags)
			return fg_IsSet(*m_ChixelFlags, EChixelFlag::mc_Italic);
		else
			return false;
	}

	bool CSetChixelProperties::f_Strikeout() const
	{
		if (m_ChixelFlags)
			return fg_IsSet(*m_ChixelFlags, EChixelFlag::mc_Strikeout);
		else
			return false;
	}

	void CSetChixelProperties::f_Weight(CAnsiEncoding::EWeight _Value)
	{
		f_ChangeFlags(EChixelFlag::mc_WeightMask, EChixelFlag(_Value) << int(EChixelFlag::mc_WeightOffset));
	}

	void CSetChixelProperties::f_Underline(CAnsiEncoding::EUnderline _Value)
	{
		f_ChangeFlags(EChixelFlag::mc_UnderlineMask, EChixelFlag(_Value) << int(EChixelFlag::mc_UnderlineOffset));
	}

	void CSetChixelProperties::f_Italic(bool _bValue)
	{
		f_ChangeFlags(EChixelFlag::mc_Italic, _bValue ? EChixelFlag::mc_Italic : EChixelFlag::mc_None);
	}

	void CSetChixelProperties::f_Strikeout(bool _bValue)
	{
		f_ChangeFlags(EChixelFlag::mc_Strikeout, _bValue ? EChixelFlag::mc_Strikeout : EChixelFlag::mc_None);
	}

	CAnsiEncoding::EWeight CChixelProperties::fs_Weight(EChixelFlag _Flags)
	{
		return CAnsiEncoding::EWeight((_Flags & EChixelFlag::mc_WeightMask) >> int(EChixelFlag::mc_WeightOffset));
	}

	CAnsiEncoding::EUnderline CChixelProperties::fs_Underline(EChixelFlag _Flags)
	{
		return CAnsiEncoding::EUnderline((_Flags & EChixelFlag::mc_UnderlineMask) >> int(EChixelFlag::mc_UnderlineOffset));
	}

	EChixelFlag CChixelProperties::fs_WeightToFlags(CAnsiEncoding::EWeight _Weight)
	{
		return EChixelFlag(_Weight) << int(EChixelFlag::mc_WeightOffset);
	}

	EChixelFlag CChixelProperties::fs_UnderlineToFlags(CAnsiEncoding::EUnderline _Underline)
	{
		return EChixelFlag(_Underline) << int(EChixelFlag::mc_UnderlineOffset);
	}

	NStr::CStr CChixelProperties::fs_WeightToString(CAnsiEncoding::EWeight _Weight)
	{
		switch (_Weight)
		{
		case CAnsiEncoding::EWeight::mc_Normal: return NStr::gc_Str<"Normal">;
		case CAnsiEncoding::EWeight::mc_Bold: return NStr::gc_Str<"Bold">;
		case CAnsiEncoding::EWeight::mc_Dim: return NStr::gc_Str<"Dim">;
		case CAnsiEncoding::EWeight::mc_Shadowed: return NStr::gc_Str<"Shadowed">;
		}

		return {};
	}

	NStr::CStr CChixelProperties::fs_UnderlineToString(CAnsiEncoding::EUnderline _Underline)
	{
		switch (_Underline)
		{
		case CAnsiEncoding::EUnderline::mc_None: return NStr::gc_Str<"None">;
		case CAnsiEncoding::EUnderline::mc_Solid: return NStr::gc_Str<"Solid">;
		case CAnsiEncoding::EUnderline::mc_Double: return NStr::gc_Str<"Double">;
		case CAnsiEncoding::EUnderline::mc_Wavy: return NStr::gc_Str<"Wavy">;
		case CAnsiEncoding::EUnderline::mc_Dotted: return NStr::gc_Str<"Dotted">;
		case CAnsiEncoding::EUnderline::mc_Dashed: return NStr::gc_Str<"Dashed">;
		}

		return {};
	}

	CAnsiEncoding::EWeight CChixelProperties::f_Weight() const
	{
		return CAnsiEncoding::EWeight((m_Flags & EChixelFlag::mc_WeightMask) >> int(EChixelFlag::mc_WeightOffset));
	}

	CAnsiEncoding::EUnderline CChixelProperties::f_Underline() const
	{
		return CAnsiEncoding::EUnderline((m_Flags & EChixelFlag::mc_UnderlineMask) >> int(EChixelFlag::mc_UnderlineOffset));
	}

	bool CChixelProperties::f_Italic() const
	{
		return fg_IsSet(m_Flags, EChixelFlag::mc_Italic);
	}

	bool CChixelProperties::f_Strikeout() const
	{
		return fg_IsSet(m_Flags, EChixelFlag::mc_Strikeout);
	}

	void CChixelProperties::f_Weight(CAnsiEncoding::EWeight _Value)
	{
		f_ChangeFlags(EChixelFlag::mc_WeightMask, EChixelFlag(_Value) << int(EChixelFlag::mc_WeightOffset));
	}

	void CChixelProperties::f_Underline(CAnsiEncoding::EUnderline _Value)
	{
		f_ChangeFlags(EChixelFlag::mc_UnderlineMask, EChixelFlag(_Value) << int(EChixelFlag::mc_UnderlineOffset));
	}

	void CChixelProperties::f_Italic(bool _bValue)
	{
		f_ChangeFlags(EChixelFlag::mc_Italic, _bValue ? EChixelFlag::mc_Italic : EChixelFlag::mc_None);
	}

	void CChixelProperties::f_Strikeout(bool _bValue)
	{
		f_ChangeFlags(EChixelFlag::mc_Strikeout, _bValue ? EChixelFlag::mc_Strikeout : EChixelFlag::mc_None);
	}

	CAnsiEncoding::EWeight CChixel::f_Weight() const
	{
		return CAnsiEncoding::EWeight((f_GetFlags() & EChixelFlag::mc_WeightMask) >> int(EChixelFlag::mc_WeightOffset));
	}

	CAnsiEncoding::EUnderline CChixel::f_Underline() const
	{
		return CAnsiEncoding::EUnderline((f_GetFlags() & EChixelFlag::mc_UnderlineMask) >> int(EChixelFlag::mc_UnderlineOffset));
	}

	bool CChixel::f_Italic() const
	{
		return fg_IsSet(f_GetFlags(), EChixelFlag::mc_Italic);
	}

	bool CChixel::f_Strikeout() const
	{
		return fg_IsSet(f_GetFlags(), EChixelFlag::mc_Strikeout);
	}

	void CChixel::f_Weight(CAnsiEncoding::EWeight _Value)
	{
		f_ChangeFlags(EChixelFlag::mc_WeightMask, EChixelFlag(_Value) << int(EChixelFlag::mc_WeightOffset));
	}

	void CChixel::f_Underline(CAnsiEncoding::EUnderline _Value)
	{
		f_ChangeFlags(EChixelFlag::mc_UnderlineMask, EChixelFlag(_Value) << int(EChixelFlag::mc_UnderlineOffset));
	}

	void CChixel::f_Italic(bool _bValue)
	{
		f_ChangeFlags(EChixelFlag::mc_Italic, _bValue ? EChixelFlag::mc_Italic : EChixelFlag::mc_None);
	}

	void CChixel::f_Strikeout(bool _bValue)
	{
		f_ChangeFlags(EChixelFlag::mc_Strikeout, _bValue ? EChixelFlag::mc_Strikeout : EChixelFlag::mc_None);
	}
}
