// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <Mib/Core/Core>
#include <Mib/CommandLine/AnsiEncodingParse>

#include "Malterlib_CommandLine_Terminal.h"

namespace NMib::NCommandLine
{
	using namespace NStr;

	void CTerminalSurface::f_GradientVertical(CTerminalArea const &_Area, CTerminalSurfaceFillProperties const &_PropertiesStart, CTerminalSurfaceFillProperties const &_PropertiesEnd)
	{
		CTerminalSurfaceFillProperties PropertiesStart = _PropertiesStart;
		CTerminalSurfaceFillProperties PropertiesEnd = _PropertiesEnd;

		if (PropertiesStart.m_Character && !PropertiesEnd.m_Character)
			PropertiesEnd.m_Character = PropertiesStart.m_Character;
		else if (!PropertiesStart.m_Character && PropertiesEnd.m_Character)
			PropertiesStart.m_Character = PropertiesEnd.m_Character;

		DMibRequire(!PropertiesStart.m_Character || (*PropertiesStart.m_Character == *PropertiesEnd.m_Character));

		if (PropertiesStart.m_Properties.m_ChixelFlags && !PropertiesEnd.m_Properties.m_ChixelFlags)
			PropertiesEnd.m_Properties.m_ChixelFlags = PropertiesStart.m_Properties.m_ChixelFlags;
		else if (!PropertiesStart.m_Properties.m_ChixelFlags && PropertiesEnd.m_Properties.m_ChixelFlags)
			PropertiesStart.m_Properties.m_ChixelFlags = PropertiesEnd.m_Properties.m_ChixelFlags;

		if (PropertiesStart.m_Properties.m_ForegroundBlend && !PropertiesEnd.m_Properties.m_ForegroundBlend)
			PropertiesEnd.m_Properties.m_ForegroundBlend = PropertiesStart.m_Properties.m_ForegroundBlend;
		else if (!PropertiesStart.m_Properties.m_ForegroundBlend && PropertiesEnd.m_Properties.m_ForegroundBlend)
			PropertiesStart.m_Properties.m_ForegroundBlend = PropertiesEnd.m_Properties.m_ForegroundBlend;

		if (PropertiesStart.m_Properties.m_BackgroundBlend && !PropertiesEnd.m_Properties.m_BackgroundBlend)
			PropertiesEnd.m_Properties.m_BackgroundBlend = PropertiesStart.m_Properties.m_BackgroundBlend;
		else if (!PropertiesStart.m_Properties.m_BackgroundBlend && PropertiesEnd.m_Properties.m_BackgroundBlend)
			PropertiesStart.m_Properties.m_BackgroundBlend = PropertiesEnd.m_Properties.m_BackgroundBlend;

		if (PropertiesStart.m_Properties.m_UnderlineBlend && !PropertiesEnd.m_Properties.m_UnderlineBlend)
			PropertiesEnd.m_Properties.m_UnderlineBlend = PropertiesStart.m_Properties.m_UnderlineBlend;
		else if (!PropertiesStart.m_Properties.m_UnderlineBlend && PropertiesEnd.m_Properties.m_UnderlineBlend)
			PropertiesStart.m_Properties.m_UnderlineBlend = PropertiesEnd.m_Properties.m_UnderlineBlend;

		DMibRequire(!PropertiesStart.m_Properties.m_ChixelFlags || (*PropertiesStart.m_Properties.m_ChixelFlags == *PropertiesEnd.m_Properties.m_ChixelFlags));
		DMibRequire(!PropertiesStart.m_Properties.m_ForegroundBlend || (*PropertiesStart.m_Properties.m_ForegroundBlend == *PropertiesEnd.m_Properties.m_ForegroundBlend));
		DMibRequire(!PropertiesStart.m_Properties.m_BackgroundBlend || (*PropertiesStart.m_Properties.m_BackgroundBlend == *PropertiesEnd.m_Properties.m_BackgroundBlend));
		DMibRequire(!PropertiesStart.m_Properties.m_UnderlineBlend || (*PropertiesStart.m_Properties.m_UnderlineBlend == *PropertiesEnd.m_Properties.m_UnderlineBlend));

		if (PropertiesStart.m_Properties.m_ForegroundColor && !PropertiesEnd.m_Properties.m_ForegroundColor)
			PropertiesEnd.m_Properties.m_ForegroundColor = PropertiesStart.m_Properties.m_ForegroundColor;
		else if (!PropertiesStart.m_Properties.m_ForegroundColor && PropertiesEnd.m_Properties.m_ForegroundColor)
			PropertiesStart.m_Properties.m_ForegroundColor = PropertiesEnd.m_Properties.m_ForegroundColor;

		if (PropertiesStart.m_Properties.m_BackgroundColor && !PropertiesEnd.m_Properties.m_BackgroundColor)
			PropertiesEnd.m_Properties.m_BackgroundColor = PropertiesStart.m_Properties.m_BackgroundColor;
		else if (!PropertiesStart.m_Properties.m_BackgroundColor && PropertiesEnd.m_Properties.m_BackgroundColor)
			PropertiesStart.m_Properties.m_BackgroundColor = PropertiesEnd.m_Properties.m_BackgroundColor;

		if (PropertiesStart.m_Properties.m_UnderlineColor && !PropertiesEnd.m_Properties.m_UnderlineColor)
			PropertiesEnd.m_Properties.m_UnderlineColor = PropertiesStart.m_Properties.m_UnderlineColor;
		else if (!PropertiesStart.m_Properties.m_UnderlineColor && PropertiesEnd.m_Properties.m_UnderlineColor)
			PropertiesStart.m_Properties.m_UnderlineColor = PropertiesEnd.m_Properties.m_UnderlineColor;

		CTerminalArea Area = _Area.f_Intersection(mp_Size);
		if (Area.m_Size.m_Height <= 0 || Area.m_Size.m_Width <= 0)
			return;

		auto LowerRight = Area.f_LowerRight();
		auto pBuffer = mp_Buffer.f_GetArray();

		int32 SurfaceWidth = mp_Size.m_Width;

		// Interpolate over size - 1 steps so the last cell reaches the end color; a single cell keeps the start color
		fp32 AmountPerCoord = fp64(0.0);
		if (_Area.m_Size.m_Height > 1)
			AmountPerCoord = fp64(1.0) / fp64(_Area.m_Size.m_Height - 1);

		fp32 AmountStart = fp64(Area.m_UpperLeft.m_CoordY - _Area.m_UpperLeft.m_CoordY) * AmountPerCoord;

		auto fFill = [&]<bool t_bCharacter, NPrivate::EColorOperation t_Foreground, NPrivate::EColorOperation t_Background, NPrivate::EColorOperation t_Underline, bool t_bFlags>
			{
			 	fp32 Amount = AmountStart;
				for (int32 CoordY = Area.m_UpperLeft.m_CoordY; CoordY < LowerRight.m_CoordY; ++CoordY, Amount += AmountPerCoord)
				{
					CTerminalColor ForegroundColor;
					if constexpr (t_Foreground != NPrivate::EColorOperation::mc_None)
					{
						auto ColorRed = (fp64(PropertiesStart.m_Properties.m_ForegroundColor->f_Red()) * (fp64(1.0) - Amount))
							+ (fp64(PropertiesEnd.m_Properties.m_ForegroundColor->f_Red()) * Amount)
						;
						auto ColorGreen = (fp64(PropertiesStart.m_Properties.m_ForegroundColor->f_Green()) * (fp64(1.0) - Amount))
							+ (fp64(PropertiesEnd.m_Properties.m_ForegroundColor->f_Green()) * Amount)
						;
						auto ColorBlue = (fp64(PropertiesStart.m_Properties.m_ForegroundColor->f_Blue()) * (fp64(1.0) - Amount))
							+ (fp64(PropertiesEnd.m_Properties.m_ForegroundColor->f_Blue()) * Amount)
						;
						auto ColorAlpha = (fp64(PropertiesStart.m_Properties.m_ForegroundColor->f_Alpha()) * (fp64(1.0) - Amount))
							+ (fp64(PropertiesEnd.m_Properties.m_ForegroundColor->f_Alpha()) * Amount)
						;
						ForegroundColor = CTerminalColor::fs_FromRGBA(ColorRed.f_ToIntRound(), ColorGreen.f_ToIntRound(), ColorBlue.f_ToIntRound(), ColorAlpha.f_ToIntRound());
					}

					CTerminalColor BackgroundColor;
					if constexpr (t_Background != NPrivate::EColorOperation::mc_None)
					{
						auto ColorRed = (fp64(PropertiesStart.m_Properties.m_BackgroundColor->f_Red()) * (fp64(1.0) - Amount))
							+ (fp64(PropertiesEnd.m_Properties.m_BackgroundColor->f_Red()) * Amount)
						;
						auto ColorGreen = (fp64(PropertiesStart.m_Properties.m_BackgroundColor->f_Green()) * (fp64(1.0) - Amount))
							+ (fp64(PropertiesEnd.m_Properties.m_BackgroundColor->f_Green()) * Amount)
						;
						auto ColorBlue = (fp64(PropertiesStart.m_Properties.m_BackgroundColor->f_Blue()) * (fp64(1.0) - Amount))
							+ (fp64(PropertiesEnd.m_Properties.m_BackgroundColor->f_Blue()) * Amount)
						;
						auto ColorAlpha = (fp64(PropertiesStart.m_Properties.m_BackgroundColor->f_Alpha()) * (fp64(1.0) - Amount))
							+ (fp64(PropertiesEnd.m_Properties.m_BackgroundColor->f_Alpha()) * Amount)
						;

						BackgroundColor = CTerminalColor::fs_FromRGBA(ColorRed.f_ToIntRound(), ColorGreen.f_ToIntRound(), ColorBlue.f_ToIntRound(), ColorAlpha.f_ToIntRound());
					}

					CTerminalColor UnderlineColor;
					if constexpr (t_Underline != NPrivate::EColorOperation::mc_None)
					{
						auto ColorRed = (fp64(PropertiesStart.m_Properties.m_UnderlineColor->f_Red()) * (fp64(1.0) - Amount))
							+ (fp64(PropertiesEnd.m_Properties.m_UnderlineColor->f_Red()) * Amount)
						;
						auto ColorGreen = (fp64(PropertiesStart.m_Properties.m_UnderlineColor->f_Green()) * (fp64(1.0) - Amount))
							+ (fp64(PropertiesEnd.m_Properties.m_UnderlineColor->f_Green()) * Amount)
						;
						auto ColorBlue = (fp64(PropertiesStart.m_Properties.m_UnderlineColor->f_Blue()) * (fp64(1.0) - Amount))
							+ (fp64(PropertiesEnd.m_Properties.m_UnderlineColor->f_Blue()) * Amount)
						;
						auto ColorAlpha = (fp64(PropertiesStart.m_Properties.m_UnderlineColor->f_Alpha()) * (fp64(1.0) - Amount))
							+ (fp64(PropertiesEnd.m_Properties.m_UnderlineColor->f_Alpha()) * Amount)
						;
						UnderlineColor = CTerminalColor::fs_FromRGBA(ColorRed.f_ToIntRound(), ColorGreen.f_ToIntRound(), ColorBlue.f_ToIntRound(), ColorAlpha.f_ToIntRound());
					}

					for (int32 CoordX = Area.m_UpperLeft.m_CoordX; CoordX < LowerRight.m_CoordX; ++CoordX)
					{
						auto &Chixel = pBuffer[CoordY * SurfaceWidth + CoordX];

						if constexpr (t_bCharacter && t_bFlags)
							Chixel.f_SetAll(*PropertiesStart.m_Character, *PropertiesStart.m_Properties.m_ChixelFlags);
						else
						{
							if constexpr (t_bCharacter)
								Chixel.f_SetCharacterAndEndpointType(*PropertiesStart.m_Character);
							if constexpr (t_bFlags)
								Chixel.f_SetFlags(*PropertiesStart.m_Properties.m_ChixelFlags);
						}

						if constexpr (t_Foreground == NPrivate::EColorOperation::mc_Overwrite)
							Chixel.m_ForegroundColor = ForegroundColor;
						else if constexpr (t_Foreground == NPrivate::EColorOperation::mc_Blend)
							Chixel.m_ForegroundColor = ForegroundColor.f_Blend(Chixel.m_ForegroundColor, *PropertiesStart.m_Properties.m_ForegroundBlend);

						if constexpr (t_Background == NPrivate::EColorOperation::mc_Overwrite)
							Chixel.m_BackgroundColor = BackgroundColor;
						else if constexpr (t_Background == NPrivate::EColorOperation::mc_Blend)
							Chixel.m_BackgroundColor = BackgroundColor.f_Blend(Chixel.m_BackgroundColor, *PropertiesStart.m_Properties.m_BackgroundBlend);

						if constexpr (t_Underline == NPrivate::EColorOperation::mc_Overwrite)
							Chixel.m_UnderlineColor = UnderlineColor;
						else if constexpr (t_Underline == NPrivate::EColorOperation::mc_Blend)
							Chixel.m_UnderlineColor = UnderlineColor.f_Blend(Chixel.m_UnderlineColor, *PropertiesStart.m_Properties.m_UnderlineBlend);
					}
				}
			}
		;

		NPrivate::fg_CallFillFunction(fFill, PropertiesStart);
	}

	void CTerminalSurface::f_GradientHorizontal(CTerminalArea const &_Area, CTerminalSurfaceFillProperties const &_PropertiesStart, CTerminalSurfaceFillProperties const &_PropertiesEnd)
	{
		CTerminalSurfaceFillProperties PropertiesStart = _PropertiesStart;
		CTerminalSurfaceFillProperties PropertiesEnd = _PropertiesEnd;

		if (PropertiesStart.m_Character && !PropertiesEnd.m_Character)
			PropertiesEnd.m_Character = PropertiesStart.m_Character;
		else if (!PropertiesStart.m_Character && PropertiesEnd.m_Character)
			PropertiesStart.m_Character = PropertiesEnd.m_Character;

		DMibRequire(!PropertiesStart.m_Character || (*PropertiesStart.m_Character == *PropertiesEnd.m_Character));

		if (PropertiesStart.m_Properties.m_ChixelFlags && !PropertiesEnd.m_Properties.m_ChixelFlags)
			PropertiesEnd.m_Properties.m_ChixelFlags = PropertiesStart.m_Properties.m_ChixelFlags;
		else if (!PropertiesStart.m_Properties.m_ChixelFlags && PropertiesEnd.m_Properties.m_ChixelFlags)
			PropertiesStart.m_Properties.m_ChixelFlags = PropertiesEnd.m_Properties.m_ChixelFlags;

		if (PropertiesStart.m_Properties.m_ForegroundBlend && !PropertiesEnd.m_Properties.m_ForegroundBlend)
			PropertiesEnd.m_Properties.m_ForegroundBlend = PropertiesStart.m_Properties.m_ForegroundBlend;
		else if (!PropertiesStart.m_Properties.m_ForegroundBlend && PropertiesEnd.m_Properties.m_ForegroundBlend)
			PropertiesStart.m_Properties.m_ForegroundBlend = PropertiesEnd.m_Properties.m_ForegroundBlend;

		if (PropertiesStart.m_Properties.m_BackgroundBlend && !PropertiesEnd.m_Properties.m_BackgroundBlend)
			PropertiesEnd.m_Properties.m_BackgroundBlend = PropertiesStart.m_Properties.m_BackgroundBlend;
		else if (!PropertiesStart.m_Properties.m_BackgroundBlend && PropertiesEnd.m_Properties.m_BackgroundBlend)
			PropertiesStart.m_Properties.m_BackgroundBlend = PropertiesEnd.m_Properties.m_BackgroundBlend;

		if (PropertiesStart.m_Properties.m_UnderlineBlend && !PropertiesEnd.m_Properties.m_UnderlineBlend)
			PropertiesEnd.m_Properties.m_UnderlineBlend = PropertiesStart.m_Properties.m_UnderlineBlend;
		else if (!PropertiesStart.m_Properties.m_UnderlineBlend && PropertiesEnd.m_Properties.m_UnderlineBlend)
			PropertiesStart.m_Properties.m_UnderlineBlend = PropertiesEnd.m_Properties.m_UnderlineBlend;

		DMibRequire(!PropertiesStart.m_Properties.m_ChixelFlags || (*PropertiesStart.m_Properties.m_ChixelFlags == *PropertiesEnd.m_Properties.m_ChixelFlags));
		DMibRequire(!PropertiesStart.m_Properties.m_ForegroundBlend || (*PropertiesStart.m_Properties.m_ForegroundBlend == *PropertiesEnd.m_Properties.m_ForegroundBlend));
		DMibRequire(!PropertiesStart.m_Properties.m_BackgroundBlend || (*PropertiesStart.m_Properties.m_BackgroundBlend == *PropertiesEnd.m_Properties.m_BackgroundBlend));
		DMibRequire(!PropertiesStart.m_Properties.m_UnderlineBlend || (*PropertiesStart.m_Properties.m_UnderlineBlend == *PropertiesEnd.m_Properties.m_UnderlineBlend));

		if (PropertiesStart.m_Properties.m_ForegroundColor && !PropertiesEnd.m_Properties.m_ForegroundColor)
			PropertiesEnd.m_Properties.m_ForegroundColor = PropertiesStart.m_Properties.m_ForegroundColor;
		else if (!PropertiesStart.m_Properties.m_ForegroundColor && PropertiesEnd.m_Properties.m_ForegroundColor)
			PropertiesStart.m_Properties.m_ForegroundColor = PropertiesEnd.m_Properties.m_ForegroundColor;

		if (PropertiesStart.m_Properties.m_BackgroundColor && !PropertiesEnd.m_Properties.m_BackgroundColor)
			PropertiesEnd.m_Properties.m_BackgroundColor = PropertiesStart.m_Properties.m_BackgroundColor;
		else if (!PropertiesStart.m_Properties.m_BackgroundColor && PropertiesEnd.m_Properties.m_BackgroundColor)
			PropertiesStart.m_Properties.m_BackgroundColor = PropertiesEnd.m_Properties.m_BackgroundColor;

		if (PropertiesStart.m_Properties.m_UnderlineColor && !PropertiesEnd.m_Properties.m_UnderlineColor)
			PropertiesEnd.m_Properties.m_UnderlineColor = PropertiesStart.m_Properties.m_UnderlineColor;
		else if (!PropertiesStart.m_Properties.m_UnderlineColor && PropertiesEnd.m_Properties.m_UnderlineColor)
			PropertiesStart.m_Properties.m_UnderlineColor = PropertiesEnd.m_Properties.m_UnderlineColor;

		CTerminalArea Area = _Area.f_Intersection(mp_Size);
		if (Area.m_Size.m_Height <= 0 || Area.m_Size.m_Width <= 0)
			return;

		auto LowerRight = Area.f_LowerRight();
		auto pBuffer = mp_Buffer.f_GetArray();

		int32 SurfaceWidth = mp_Size.m_Width;

		// Interpolate over size - 1 steps so the last cell reaches the end color; a single cell keeps the start color
		fp32 AmountPerCoord = fp64(0.0);
		if (_Area.m_Size.m_Width > 1)
			AmountPerCoord = fp64(1.0) / fp64(_Area.m_Size.m_Width - 1);

		fp32 AmountStart = fp64(Area.m_UpperLeft.m_CoordX - _Area.m_UpperLeft.m_CoordX) * AmountPerCoord;

		auto fFill = [&]<bool t_bCharacter, NPrivate::EColorOperation t_Foreground, NPrivate::EColorOperation t_Background, NPrivate::EColorOperation t_Underline, bool t_bFlags>
			{
				for (int32 CoordY = Area.m_UpperLeft.m_CoordY; CoordY < LowerRight.m_CoordY; ++CoordY)
				{
				 	fp32 Amount = AmountStart;
					for (int32 CoordX = Area.m_UpperLeft.m_CoordX; CoordX < LowerRight.m_CoordX; ++CoordX, Amount += AmountPerCoord)
					{
						auto &Chixel = pBuffer[CoordY * SurfaceWidth + CoordX];

						if constexpr (t_bCharacter && t_bFlags)
							Chixel.f_SetAll(*PropertiesStart.m_Character, *PropertiesStart.m_Properties.m_ChixelFlags);
						else
						{
							if constexpr (t_bCharacter)
								Chixel.f_SetCharacterAndEndpointType(*PropertiesStart.m_Character);
							if constexpr (t_bFlags)
								Chixel.f_SetFlags(*PropertiesStart.m_Properties.m_ChixelFlags);
						}

						if constexpr (t_Foreground != NPrivate::EColorOperation::mc_None)
						{
							auto ColorRed = (fp64(PropertiesStart.m_Properties.m_ForegroundColor->f_Red()) * (fp64(1.0) - Amount))
								+ (fp64(PropertiesEnd.m_Properties.m_ForegroundColor->f_Red()) * Amount)
							;
							auto ColorGreen = (fp64(PropertiesStart.m_Properties.m_ForegroundColor->f_Green()) * (fp64(1.0) - Amount))
								+ (fp64(PropertiesEnd.m_Properties.m_ForegroundColor->f_Green()) * Amount)
							;
							auto ColorBlue = (fp64(PropertiesStart.m_Properties.m_ForegroundColor->f_Blue()) * (fp64(1.0) - Amount))
								+ (fp64(PropertiesEnd.m_Properties.m_ForegroundColor->f_Blue()) * Amount)
							;
							auto ColorAlpha = (fp64(PropertiesStart.m_Properties.m_ForegroundColor->f_Alpha()) * (fp64(1.0) - Amount))
								+ (fp64(PropertiesEnd.m_Properties.m_ForegroundColor->f_Alpha()) * Amount)
							;

							auto SourceColor = CTerminalColor::fs_FromRGBA(ColorRed.f_ToIntRound(), ColorGreen.f_ToIntRound(), ColorBlue.f_ToIntRound(), ColorAlpha.f_ToIntRound());

							if constexpr (t_Foreground == NPrivate::EColorOperation::mc_Overwrite)
								Chixel.m_ForegroundColor = SourceColor;
							else if constexpr (t_Foreground == NPrivate::EColorOperation::mc_Blend)
								Chixel.m_ForegroundColor = SourceColor.f_Blend(Chixel.m_ForegroundColor, *PropertiesStart.m_Properties.m_ForegroundBlend);
						}

						if constexpr (t_Background != NPrivate::EColorOperation::mc_None)
						{
							auto ColorRed = (fp64(PropertiesStart.m_Properties.m_BackgroundColor->f_Red()) * (fp64(1.0) - Amount))
								+ (fp64(PropertiesEnd.m_Properties.m_BackgroundColor->f_Red()) * Amount)
							;
							auto ColorGreen = (fp64(PropertiesStart.m_Properties.m_BackgroundColor->f_Green()) * (fp64(1.0) - Amount))
								+ (fp64(PropertiesEnd.m_Properties.m_BackgroundColor->f_Green()) * Amount)
							;
							auto ColorBlue = (fp64(PropertiesStart.m_Properties.m_BackgroundColor->f_Blue()) * (fp64(1.0) - Amount))
								+ (fp64(PropertiesEnd.m_Properties.m_BackgroundColor->f_Blue()) * Amount)
							;
							auto ColorAlpha = (fp64(PropertiesStart.m_Properties.m_BackgroundColor->f_Alpha()) * (fp64(1.0) - Amount))
								+ (fp64(PropertiesEnd.m_Properties.m_BackgroundColor->f_Alpha()) * Amount)
							;

							auto SourceColor = CTerminalColor::fs_FromRGBA(ColorRed.f_ToIntRound(), ColorGreen.f_ToIntRound(), ColorBlue.f_ToIntRound(), ColorAlpha.f_ToIntRound());

							if constexpr (t_Background == NPrivate::EColorOperation::mc_Overwrite)
								Chixel.m_BackgroundColor = SourceColor;
							else if constexpr (t_Background == NPrivate::EColorOperation::mc_Blend)
								Chixel.m_BackgroundColor = SourceColor.f_Blend(Chixel.m_BackgroundColor, *PropertiesStart.m_Properties.m_BackgroundBlend);
						}

						if constexpr (t_Underline != NPrivate::EColorOperation::mc_None)
						{
							auto ColorRed = (fp64(PropertiesStart.m_Properties.m_UnderlineColor->f_Red()) * (fp64(1.0) - Amount))
								+ (fp64(PropertiesEnd.m_Properties.m_UnderlineColor->f_Red()) * Amount)
							;
							auto ColorGreen = (fp64(PropertiesStart.m_Properties.m_UnderlineColor->f_Green()) * (fp64(1.0) - Amount))
								+ (fp64(PropertiesEnd.m_Properties.m_UnderlineColor->f_Green()) * Amount)
							;
							auto ColorBlue = (fp64(PropertiesStart.m_Properties.m_UnderlineColor->f_Blue()) * (fp64(1.0) - Amount))
								+ (fp64(PropertiesEnd.m_Properties.m_UnderlineColor->f_Blue()) * Amount)
							;
							auto ColorAlpha = (fp64(PropertiesStart.m_Properties.m_UnderlineColor->f_Alpha()) * (fp64(1.0) - Amount))
								+ (fp64(PropertiesEnd.m_Properties.m_UnderlineColor->f_Alpha()) * Amount)
							;

							auto SourceColor = CTerminalColor::fs_FromRGBA(ColorRed.f_ToIntRound(), ColorGreen.f_ToIntRound(), ColorBlue.f_ToIntRound(), ColorAlpha.f_ToIntRound());

							if constexpr (t_Underline == NPrivate::EColorOperation::mc_Overwrite)
								Chixel.m_UnderlineColor = SourceColor;
							else if constexpr (t_Underline == NPrivate::EColorOperation::mc_Blend)
								Chixel.m_UnderlineColor = SourceColor.f_Blend(Chixel.m_UnderlineColor, *PropertiesStart.m_Properties.m_UnderlineBlend);
						}
					}
				}
			}
		;

		NPrivate::fg_CallFillFunction(fFill, PropertiesStart);
	}

	void CTerminalSurface::f_Gradient
		(
			CTerminalArea const &_Area
			, CTerminalSurfaceFillProperties const &_UpperLeft
			, CTerminalSurfaceFillProperties const &_UpperRight
			, CTerminalSurfaceFillProperties const &_LowerLeft
			, CTerminalSurfaceFillProperties const &_LowerRight
		)
	{
		auto fFillProperties = [](CTerminalSurfaceFillProperties &_Left, CTerminalSurfaceFillProperties &_Right)
			{
				if (_Left.m_Character && !_Right.m_Character)
					_Right.m_Character = _Left.m_Character;
				else if (!_Left.m_Character && _Right.m_Character)
					_Left.m_Character = _Right.m_Character;

				DMibRequire(!_Left.m_Character || (*_Left.m_Character == *_Right.m_Character));

				if (_Left.m_Properties.m_ChixelFlags && !_Right.m_Properties.m_ChixelFlags)
					_Right.m_Properties.m_ChixelFlags = _Left.m_Properties.m_ChixelFlags;
				else if (!_Left.m_Properties.m_ChixelFlags && _Right.m_Properties.m_ChixelFlags)
					_Left.m_Properties.m_ChixelFlags = _Right.m_Properties.m_ChixelFlags;

				if (_Left.m_Properties.m_ForegroundBlend && !_Right.m_Properties.m_ForegroundBlend)
					_Right.m_Properties.m_ForegroundBlend = _Left.m_Properties.m_ForegroundBlend;
				else if (!_Left.m_Properties.m_ForegroundBlend && _Right.m_Properties.m_ForegroundBlend)
					_Left.m_Properties.m_ForegroundBlend = _Right.m_Properties.m_ForegroundBlend;

				if (_Left.m_Properties.m_BackgroundBlend && !_Right.m_Properties.m_BackgroundBlend)
					_Right.m_Properties.m_BackgroundBlend = _Left.m_Properties.m_BackgroundBlend;
				else if (!_Left.m_Properties.m_BackgroundBlend && _Right.m_Properties.m_BackgroundBlend)
					_Left.m_Properties.m_BackgroundBlend = _Right.m_Properties.m_BackgroundBlend;

				if (_Left.m_Properties.m_UnderlineBlend && !_Right.m_Properties.m_UnderlineBlend)
					_Right.m_Properties.m_UnderlineBlend = _Left.m_Properties.m_UnderlineBlend;
				else if (!_Left.m_Properties.m_UnderlineBlend && _Right.m_Properties.m_UnderlineBlend)
					_Left.m_Properties.m_UnderlineBlend = _Right.m_Properties.m_UnderlineBlend;

				DMibRequire(!_Left.m_Properties.m_ChixelFlags || (*_Left.m_Properties.m_ChixelFlags == *_Right.m_Properties.m_ChixelFlags));
				DMibRequire(!_Left.m_Properties.m_ForegroundBlend || (*_Left.m_Properties.m_ForegroundBlend == *_Right.m_Properties.m_ForegroundBlend));
				DMibRequire(!_Left.m_Properties.m_BackgroundBlend || (*_Left.m_Properties.m_BackgroundBlend == *_Right.m_Properties.m_BackgroundBlend));
				DMibRequire(!_Left.m_Properties.m_UnderlineBlend || (*_Left.m_Properties.m_UnderlineBlend == *_Right.m_Properties.m_UnderlineBlend));

				if (_Left.m_Properties.m_ForegroundColor && !_Right.m_Properties.m_ForegroundColor)
					_Right.m_Properties.m_ForegroundColor = _Left.m_Properties.m_ForegroundColor;
				else if (!_Left.m_Properties.m_ForegroundColor && _Right.m_Properties.m_ForegroundColor)
					_Left.m_Properties.m_ForegroundColor = _Right.m_Properties.m_ForegroundColor;

				if (_Left.m_Properties.m_BackgroundColor && !_Right.m_Properties.m_BackgroundColor)
					_Right.m_Properties.m_BackgroundColor = _Left.m_Properties.m_BackgroundColor;
				else if (!_Left.m_Properties.m_BackgroundColor && _Right.m_Properties.m_BackgroundColor)
					_Left.m_Properties.m_BackgroundColor = _Right.m_Properties.m_BackgroundColor;

				if (_Left.m_Properties.m_UnderlineColor && !_Right.m_Properties.m_UnderlineColor)
					_Right.m_Properties.m_UnderlineColor = _Left.m_Properties.m_UnderlineColor;
				else if (!_Left.m_Properties.m_UnderlineColor && _Right.m_Properties.m_UnderlineColor)
					_Left.m_Properties.m_UnderlineColor = _Right.m_Properties.m_UnderlineColor;
			}
		;

		CTerminalSurfaceFillProperties PropertiesUpperLeft = _UpperLeft;
		CTerminalSurfaceFillProperties PropertiesUpperRight = _UpperRight;
		CTerminalSurfaceFillProperties PropertiesLowerLeft = _LowerLeft;
		CTerminalSurfaceFillProperties PropertiesLowerRight = _LowerRight;

		fFillProperties(PropertiesUpperLeft, PropertiesUpperRight);
		fFillProperties(PropertiesUpperLeft, PropertiesLowerLeft);
		fFillProperties(PropertiesUpperLeft, PropertiesLowerRight);
		fFillProperties(PropertiesUpperLeft, PropertiesLowerLeft);
		fFillProperties(PropertiesUpperLeft, PropertiesUpperRight);

		CTerminalArea Area = _Area.f_Intersection(mp_Size);
		if (Area.m_Size.m_Height <= 0 || Area.m_Size.m_Width <= 0)
			return;

		auto &UpperLeft = Area.m_UpperLeft;
		auto LowerRight = Area.f_LowerRight();
		auto pBuffer = mp_Buffer.f_GetArray();

		int32 SurfaceWidth = mp_Size.m_Width;

		// A unit-sized axis has no interpolation steps and keeps the upper left corner color on that axis
		fp32 AmountPerCoordX = fp64(0.0);
		if (_Area.m_Size.m_Width > 1)
			AmountPerCoordX = fp64(1.0) / fp64(_Area.m_Size.m_Width - 1);

		fp32 AmountPerCoordY = fp64(0.0);
		if (_Area.m_Size.m_Height > 1)
			AmountPerCoordY = fp64(1.0) / fp64(_Area.m_Size.m_Height - 1);

		fp32 AmountStartX = fp64(UpperLeft.m_CoordX - _Area.m_UpperLeft.m_CoordX) * AmountPerCoordX;
		fp32 AmountStartY = fp64(UpperLeft.m_CoordY - _Area.m_UpperLeft.m_CoordY) * AmountPerCoordY;

		auto fFill = [&]<bool t_bCharacter, NPrivate::EColorOperation t_Foreground, NPrivate::EColorOperation t_Background, NPrivate::EColorOperation t_Underline, bool t_bFlags>
			{
			 	fp32 AmountY = AmountStartY;
				for (int32 CoordY = UpperLeft.m_CoordY; CoordY < LowerRight.m_CoordY; ++CoordY, AmountY += AmountPerCoordY)
				{
				 	fp32 AmountX = AmountStartX;
					for (int32 CoordX = UpperLeft.m_CoordX; CoordX < LowerRight.m_CoordX; ++CoordX, AmountX += AmountPerCoordX)
					{
						auto &Chixel = pBuffer[CoordY * SurfaceWidth + CoordX];

						if constexpr (t_bCharacter && t_bFlags)
							Chixel.f_SetAll(*PropertiesUpperLeft.m_Character, *PropertiesUpperLeft.m_Properties.m_ChixelFlags);
						else
						{
							if constexpr (t_bCharacter)
								Chixel.f_SetCharacterAndEndpointType(*PropertiesUpperLeft.m_Character);
							if constexpr (t_bFlags)
								Chixel.f_SetFlags(*PropertiesUpperLeft.m_Properties.m_ChixelFlags);
						}

						if constexpr (t_Foreground != NPrivate::EColorOperation::mc_None)
						{
							fp32 AmountXInv = fp64(1.0) - AmountX;
							fp32 AmountYInv = fp64(1.0) - AmountY;

 							auto ColorRed = (fp64(PropertiesUpperLeft.m_Properties.m_ForegroundColor->f_Red()) * AmountXInv * AmountYInv)
								+ (fp64(PropertiesUpperRight.m_Properties.m_ForegroundColor->f_Red()) * AmountX * AmountYInv)
								+ (fp64(PropertiesLowerLeft.m_Properties.m_ForegroundColor->f_Red()) * AmountXInv * AmountY)
								+ (fp64(PropertiesLowerRight.m_Properties.m_ForegroundColor->f_Red()) * AmountX * AmountY)
							;
 							auto ColorGreen = (fp64(PropertiesUpperLeft.m_Properties.m_ForegroundColor->f_Green()) * AmountXInv * AmountYInv)
								+ (fp64(PropertiesUpperRight.m_Properties.m_ForegroundColor->f_Green()) * AmountX * AmountYInv)
								+ (fp64(PropertiesLowerLeft.m_Properties.m_ForegroundColor->f_Green()) * AmountXInv * AmountY)
								+ (fp64(PropertiesLowerRight.m_Properties.m_ForegroundColor->f_Green()) * AmountX * AmountY)
							;
 							auto ColorBlue = (fp64(PropertiesUpperLeft.m_Properties.m_ForegroundColor->f_Blue()) * AmountXInv * AmountYInv)
								+ (fp64(PropertiesUpperRight.m_Properties.m_ForegroundColor->f_Blue()) * AmountX * AmountYInv)
								+ (fp64(PropertiesLowerLeft.m_Properties.m_ForegroundColor->f_Blue()) * AmountXInv * AmountY)
								+ (fp64(PropertiesLowerRight.m_Properties.m_ForegroundColor->f_Blue()) * AmountX * AmountY)
							;
 							auto ColorAlpha = (fp64(PropertiesUpperLeft.m_Properties.m_ForegroundColor->f_Alpha()) * AmountXInv * AmountYInv)
								+ (fp64(PropertiesUpperRight.m_Properties.m_ForegroundColor->f_Alpha()) * AmountX * AmountYInv)
								+ (fp64(PropertiesLowerLeft.m_Properties.m_ForegroundColor->f_Alpha()) * AmountXInv * AmountY)
								+ (fp64(PropertiesLowerRight.m_Properties.m_ForegroundColor->f_Alpha()) * AmountX * AmountY)
							;

							auto SourceColor = CTerminalColor::fs_FromRGBA(ColorRed.f_ToIntRound(), ColorGreen.f_ToIntRound(), ColorBlue.f_ToIntRound(), ColorAlpha.f_ToIntRound());

							if constexpr (t_Foreground == NPrivate::EColorOperation::mc_Overwrite)
								Chixel.m_ForegroundColor = SourceColor;
							else if constexpr (t_Foreground == NPrivate::EColorOperation::mc_Blend)
								Chixel.m_ForegroundColor = SourceColor.f_Blend(Chixel.m_ForegroundColor, *PropertiesUpperLeft.m_Properties.m_ForegroundBlend);
						}

						if constexpr (t_Background != NPrivate::EColorOperation::mc_None)
						{
							fp32 AmountXInv = fp64(1.0) - AmountX;
							fp32 AmountYInv = fp64(1.0) - AmountY;

 							auto ColorRed = (fp64(PropertiesUpperLeft.m_Properties.m_BackgroundColor->f_Red()) * AmountXInv * AmountYInv)
								+ (fp64(PropertiesUpperRight.m_Properties.m_BackgroundColor->f_Red()) * AmountX * AmountYInv)
								+ (fp64(PropertiesLowerLeft.m_Properties.m_BackgroundColor->f_Red()) * AmountXInv * AmountY)
								+ (fp64(PropertiesLowerRight.m_Properties.m_BackgroundColor->f_Red()) * AmountX * AmountY)
							;
 							auto ColorGreen = (fp64(PropertiesUpperLeft.m_Properties.m_BackgroundColor->f_Green()) * AmountXInv * AmountYInv)
								+ (fp64(PropertiesUpperRight.m_Properties.m_BackgroundColor->f_Green()) * AmountX * AmountYInv)
								+ (fp64(PropertiesLowerLeft.m_Properties.m_BackgroundColor->f_Green()) * AmountXInv * AmountY)
								+ (fp64(PropertiesLowerRight.m_Properties.m_BackgroundColor->f_Green()) * AmountX * AmountY)
							;
 							auto ColorBlue = (fp64(PropertiesUpperLeft.m_Properties.m_BackgroundColor->f_Blue()) * AmountXInv * AmountYInv)
								+ (fp64(PropertiesUpperRight.m_Properties.m_BackgroundColor->f_Blue()) * AmountX * AmountYInv)
								+ (fp64(PropertiesLowerLeft.m_Properties.m_BackgroundColor->f_Blue()) * AmountXInv * AmountY)
								+ (fp64(PropertiesLowerRight.m_Properties.m_BackgroundColor->f_Blue()) * AmountX * AmountY)
							;
 							auto ColorAlpha = (fp64(PropertiesUpperLeft.m_Properties.m_BackgroundColor->f_Alpha()) * AmountXInv * AmountYInv)
								+ (fp64(PropertiesUpperRight.m_Properties.m_BackgroundColor->f_Alpha()) * AmountX * AmountYInv)
								+ (fp64(PropertiesLowerLeft.m_Properties.m_BackgroundColor->f_Alpha()) * AmountXInv * AmountY)
								+ (fp64(PropertiesLowerRight.m_Properties.m_BackgroundColor->f_Alpha()) * AmountX * AmountY)
							;

							auto SourceColor = CTerminalColor::fs_FromRGBA(ColorRed.f_ToIntRound(), ColorGreen.f_ToIntRound(), ColorBlue.f_ToIntRound(), ColorAlpha.f_ToIntRound());

							if constexpr (t_Background == NPrivate::EColorOperation::mc_Overwrite)
								Chixel.m_BackgroundColor = SourceColor;
							else if constexpr (t_Background == NPrivate::EColorOperation::mc_Blend)
								Chixel.m_BackgroundColor = SourceColor.f_Blend(Chixel.m_BackgroundColor, *PropertiesUpperLeft.m_Properties.m_BackgroundBlend);
						}

						if constexpr (t_Underline != NPrivate::EColorOperation::mc_None)
						{
							fp32 AmountXInv = fp64(1.0) - AmountX;
							fp32 AmountYInv = fp64(1.0) - AmountY;

 							auto ColorRed = (fp64(PropertiesUpperLeft.m_Properties.m_UnderlineColor->f_Red()) * AmountXInv * AmountYInv)
								+ (fp64(PropertiesUpperRight.m_Properties.m_UnderlineColor->f_Red()) * AmountX * AmountYInv)
								+ (fp64(PropertiesLowerLeft.m_Properties.m_UnderlineColor->f_Red()) * AmountXInv * AmountY)
								+ (fp64(PropertiesLowerRight.m_Properties.m_UnderlineColor->f_Red()) * AmountX * AmountY)
							;
 							auto ColorGreen = (fp64(PropertiesUpperLeft.m_Properties.m_UnderlineColor->f_Green()) * AmountXInv * AmountYInv)
								+ (fp64(PropertiesUpperRight.m_Properties.m_UnderlineColor->f_Green()) * AmountX * AmountYInv)
								+ (fp64(PropertiesLowerLeft.m_Properties.m_UnderlineColor->f_Green()) * AmountXInv * AmountY)
								+ (fp64(PropertiesLowerRight.m_Properties.m_UnderlineColor->f_Green()) * AmountX * AmountY)
							;
 							auto ColorBlue = (fp64(PropertiesUpperLeft.m_Properties.m_UnderlineColor->f_Blue()) * AmountXInv * AmountYInv)
								+ (fp64(PropertiesUpperRight.m_Properties.m_UnderlineColor->f_Blue()) * AmountX * AmountYInv)
								+ (fp64(PropertiesLowerLeft.m_Properties.m_UnderlineColor->f_Blue()) * AmountXInv * AmountY)
								+ (fp64(PropertiesLowerRight.m_Properties.m_UnderlineColor->f_Blue()) * AmountX * AmountY)
							;
 							auto ColorAlpha = (fp64(PropertiesUpperLeft.m_Properties.m_UnderlineColor->f_Alpha()) * AmountXInv * AmountYInv)
								+ (fp64(PropertiesUpperRight.m_Properties.m_UnderlineColor->f_Alpha()) * AmountX * AmountYInv)
								+ (fp64(PropertiesLowerLeft.m_Properties.m_UnderlineColor->f_Alpha()) * AmountXInv * AmountY)
								+ (fp64(PropertiesLowerRight.m_Properties.m_UnderlineColor->f_Alpha()) * AmountX * AmountY)
							;

							auto SourceColor = CTerminalColor::fs_FromRGBA(ColorRed.f_ToIntRound(), ColorGreen.f_ToIntRound(), ColorBlue.f_ToIntRound(), ColorAlpha.f_ToIntRound());

							if constexpr (t_Underline == NPrivate::EColorOperation::mc_Overwrite)
								Chixel.m_UnderlineColor = SourceColor;
							else if constexpr (t_Underline == NPrivate::EColorOperation::mc_Blend)
								Chixel.m_UnderlineColor = SourceColor.f_Blend(Chixel.m_UnderlineColor, *PropertiesUpperLeft.m_Properties.m_UnderlineBlend);
						}
					}
				}
			}
		;

		NPrivate::fg_CallFillFunction(fFill, PropertiesUpperLeft);
	}
}
