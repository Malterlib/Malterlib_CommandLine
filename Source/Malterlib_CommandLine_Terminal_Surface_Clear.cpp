// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <Mib/Core/Core>
#include <Mib/CommandLine/AnsiEncodingParse>

#include "Malterlib_CommandLine_Terminal.h"

namespace NMib::NCommandLine
{
	using namespace NStr;

	void CTerminalSurface::f_Clear(CTerminalSurfaceClearProperties const &_ClearProperties)
	{
		CChixelProperties DefaultProperties;
		DefaultProperties.m_BackgroundColor = CTerminalColor::fs_FromRGBA(0, 0, 0);
		DefaultProperties.m_ForegroundColor = CTerminalColor::fs_FromRGBA(255, 255, 255);
		DefaultProperties.m_UnderlineColor = DefaultProperties.m_ForegroundColor;
		DefaultProperties.m_Flags = EChixelFlag::mc_None;

		auto ClearChar = _ClearProperties.m_ClearCharacter;

		if (_ClearProperties.m_ClearProperties.m_BackgroundColor)
			DefaultProperties.m_BackgroundColor = *_ClearProperties.m_ClearProperties.m_BackgroundColor;

		if (_ClearProperties.m_ClearProperties.m_ForegroundColor)
			DefaultProperties.m_ForegroundColor = *_ClearProperties.m_ClearProperties.m_ForegroundColor;

		if (_ClearProperties.m_ClearProperties.m_UnderlineColor)
			DefaultProperties.m_UnderlineColor = *_ClearProperties.m_ClearProperties.m_UnderlineColor;

		if (_ClearProperties.m_ClearProperties.m_ChixelFlags)
			DefaultProperties.m_Flags = *_ClearProperties.m_ClearProperties.m_ChixelFlags;

		mp_DefaultProperties = DefaultProperties;

#if DCommandLineSupportOverflow
		mp_OverflowStrings.f_Clear();
#endif

		for (auto &Chixel : mp_Buffer)
		{
			Chixel.f_SetAll(ClearChar, DefaultProperties.m_Flags);
			Chixel.m_BackgroundColor = DefaultProperties.m_BackgroundColor;
			Chixel.m_ForegroundColor = DefaultProperties.m_ForegroundColor;
			Chixel.m_UnderlineColor = DefaultProperties.m_UnderlineColor;
		}
	}
}
