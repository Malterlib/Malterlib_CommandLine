// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <Mib/Core/Core>
#include <Mib/CommandLine/AnsiEncodingParse>

#include "Malterlib_CommandLine_Terminal.h"

namespace NMib::NCommandLine
{
	using namespace NStr;

	void CTerminalSurface::f_Fill(CTerminalArea const &_Area, CTerminalSurfaceFillProperties const &_Properties)
	{
		CTerminalArea Area = _Area.f_Intersection(mp_Size);

		if (Area.m_Size.m_Height <= 0 || Area.m_Size.m_Width <= 0)
			return;

		auto &UpperLeft = Area.m_UpperLeft;
		auto LowerRight = Area.f_LowerRight();
		auto pBuffer = mp_Buffer.f_GetArray();

		int32 SurfaceWidth = mp_Size.m_Width;

		CTerminalColor BackgroundColor;
		CTerminalColor ForegroundColor;
		CTerminalColor UnderlineColor;

		if (_Properties.m_Properties.m_BackgroundColor)
			BackgroundColor = *_Properties.m_Properties.m_BackgroundColor;
		if (_Properties.m_Properties.m_ForegroundColor)
			ForegroundColor = *_Properties.m_Properties.m_ForegroundColor;
		if (_Properties.m_Properties.m_UnderlineColor)
			UnderlineColor = *_Properties.m_Properties.m_UnderlineColor;

		auto fFill = [&]<bool tf_bCharacter, NPrivate::EColorOperation tf_Foreground, NPrivate::EColorOperation tf_Background, NPrivate::EColorOperation tf_Underline, bool tf_bFlags>
			{
				for (int32 CoordY = UpperLeft.m_CoordY; CoordY < LowerRight.m_CoordY; ++CoordY)
				{
					for (int32 CoordX = UpperLeft.m_CoordX; CoordX < LowerRight.m_CoordX; ++CoordX)
					{
						auto &Chixel = pBuffer[CoordY * SurfaceWidth + CoordX];

						if constexpr (tf_bCharacter && tf_bFlags)
							Chixel.f_SetAll(*_Properties.m_Character, *_Properties.m_Properties.m_ChixelFlags);
						else
						{
							if constexpr (tf_bCharacter)
								Chixel.f_SetCharacterAndEndpointType(*_Properties.m_Character);
							if constexpr (tf_bFlags)
								Chixel.f_SetFlags(*_Properties.m_Properties.m_ChixelFlags);
						}

						if constexpr (tf_Foreground == NPrivate::EColorOperation::mc_Overwrite)
							Chixel.m_ForegroundColor = ForegroundColor;
						else if constexpr (tf_Foreground == NPrivate::EColorOperation::mc_Blend)
							Chixel.m_ForegroundColor = ForegroundColor.f_Blend(Chixel.m_ForegroundColor, *_Properties.m_Properties.m_ForegroundBlend);

						if constexpr (tf_Background == NPrivate::EColorOperation::mc_Overwrite)
							Chixel.m_BackgroundColor = BackgroundColor;
						else if constexpr (tf_Background == NPrivate::EColorOperation::mc_Blend)
							Chixel.m_BackgroundColor = BackgroundColor.f_Blend(Chixel.m_BackgroundColor, *_Properties.m_Properties.m_BackgroundBlend);

						if constexpr (tf_Underline == NPrivate::EColorOperation::mc_Overwrite)
							Chixel.m_UnderlineColor = UnderlineColor;
						else if constexpr (tf_Underline == NPrivate::EColorOperation::mc_Blend)
							Chixel.m_UnderlineColor = UnderlineColor.f_Blend(Chixel.m_UnderlineColor, *_Properties.m_Properties.m_UnderlineBlend);
					}
				}
			}
		;

		NPrivate::fg_CallFillFunction(fFill, _Properties);
	}
}
