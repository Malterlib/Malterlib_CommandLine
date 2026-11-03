// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

namespace NMib::NCommandLine::NPrivate
{
	enum class EColorOperation
	{
		mc_None
		, mc_Overwrite
		, mc_Blend
	};

	template <bool tf_bCharacter, EColorOperation tf_Foreground, EColorOperation tf_Background, EColorOperation tf_Underline, bool tf_bFlags, typename tf_CFill>
	inline_always void fg_CallFillFunctionFull(tf_CFill &&_fFill, CTerminalSurfaceFillProperties const &_Properties)
	{
		_fFill.template operator ()<tf_bCharacter, tf_Foreground, tf_Background, tf_Underline, tf_bFlags>();
	}

	template <bool tf_bCharacter, EColorOperation tf_Foreground, EColorOperation tf_Background, EColorOperation tf_Underline, typename tf_CFill>
	inline_always void fg_CallFillFunction4(tf_CFill &&_fFill, CTerminalSurfaceFillProperties const &_Properties)
	{
		if (_Properties.m_Properties.m_ChixelFlags)
			fg_CallFillFunctionFull<tf_bCharacter, tf_Foreground, tf_Background, tf_Underline, true>(_fFill, _Properties);
		else
			fg_CallFillFunctionFull<tf_bCharacter, tf_Foreground, tf_Background, tf_Underline, false>(_fFill, _Properties);
	}

	template <bool tf_bCharacter, EColorOperation tf_Foreground, EColorOperation tf_Background, typename tf_CFill>
	inline_always void fg_CallFillFunction3(tf_CFill &&_fFill, CTerminalSurfaceFillProperties const &_Properties)
	{
		if (_Properties.m_Properties.m_UnderlineColor)
		{
			if (_Properties.m_Properties.m_UnderlineBlend)
				fg_CallFillFunction4<tf_bCharacter, tf_Foreground, tf_Background, EColorOperation::mc_Blend>(_fFill, _Properties);
			else
				fg_CallFillFunction4<tf_bCharacter, tf_Foreground, tf_Background, EColorOperation::mc_Overwrite>(_fFill, _Properties);
		}
		else
			fg_CallFillFunction4<tf_bCharacter, tf_Foreground, tf_Background, EColorOperation::mc_None>(_fFill, _Properties);
	}

	template <bool tf_bCharacter, EColorOperation tf_Foreground, typename tf_CFill>
	inline_always void fg_CallFillFunction2(tf_CFill &&_fFill, CTerminalSurfaceFillProperties const &_Properties)
	{
		if (_Properties.m_Properties.m_BackgroundColor)
		{
			if (_Properties.m_Properties.m_BackgroundBlend)
				fg_CallFillFunction3<tf_bCharacter, tf_Foreground, EColorOperation::mc_Blend>(_fFill, _Properties);
			else
				fg_CallFillFunction3<tf_bCharacter, tf_Foreground, EColorOperation::mc_Overwrite>(_fFill, _Properties);
		}
		else
			fg_CallFillFunction3<tf_bCharacter, tf_Foreground, EColorOperation::mc_None>(_fFill, _Properties);
	}

	template <bool tf_bCharacter, typename tf_CFill>
	inline_always void fg_CallFillFunction1(tf_CFill &&_fFill, CTerminalSurfaceFillProperties const &_Properties)
	{
		if (_Properties.m_Properties.m_ForegroundColor)
		{
			if (_Properties.m_Properties.m_ForegroundBlend)
				fg_CallFillFunction2<tf_bCharacter, EColorOperation::mc_Blend>(_fFill, _Properties);
			else
				fg_CallFillFunction2<tf_bCharacter, EColorOperation::mc_Overwrite>(_fFill, _Properties);
		}
		else
			fg_CallFillFunction2<tf_bCharacter, EColorOperation::mc_None>(_fFill, _Properties);
	}

	template <typename tf_CFill>
	inline_always void fg_CallFillFunction(tf_CFill &&_fFill, CTerminalSurfaceFillProperties const &_Properties)
	{
		if (_Properties.m_Character)
			fg_CallFillFunction1<true>(_fFill, _Properties);
		else
			fg_CallFillFunction1<false>(_fFill, _Properties);
	}
}
