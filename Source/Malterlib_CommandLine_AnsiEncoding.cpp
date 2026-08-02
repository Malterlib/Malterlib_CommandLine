// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "Malterlib_CommandLine_AnsiEncoding.h"
#include "Malterlib_CommandLine_AnsiEncodingParse.h"

#include <Mib/Container/Regions>
#include <Mib/Graphics/ColorR8G8B8>
#include <Mib/Storage/Optional>
#include <Mib/String/Appender>

namespace NMib::NCommandLine
{
	extern CAnsiEncodingParse::CDecodedColor g_CommandLine_AnsiEncodingColor256Array[256];

	using namespace NStr;
	using namespace NFunction;
	using namespace NContainer;
	using namespace NStorage;

	#define DMibCommandLineAnsiColor_Reset "\x1B[0m"
	#define DMibCommandLineAnsiColor_Bold "\x1B[1m"
	#define DMibCommandLineAnsiColor_Reverse "\x1B[7m"
	#define DMibCommandLineAnsiColor_256(d_Color) "\x1B[38;5;" #d_Color "m"

	namespace
	{
		constexpr NStr::CStr gc_Empty;
		constexpr NStr::CStr gc_Default = gc_Str<DMibCommandLineAnsiColor_Reset>;
		constexpr NStr::CStr gc_Reset = gc_Str<"\x1B""c">;
		constexpr NStr::CStr gc_UpperLeft = gc_Str<"\x1B[H">;
		constexpr NStr::CStr gc_ClearToEndOfScreen = gc_Str<"\x1B[J">;

		constexpr NStr::CStr gc_StatusNormal = gc_Str<DMibCommandLineAnsiColor_Reset DMibCommandLineAnsiColor_256(118)>;
		constexpr NStr::CStr gc_StatusWarning = gc_Str<DMibCommandLineAnsiColor_Reset DMibCommandLineAnsiColor_256(207)>;
		constexpr NStr::CStr gc_StatusError = gc_Str<DMibCommandLineAnsiColor_Reset DMibCommandLineAnsiColor_Bold DMibCommandLineAnsiColor_256(198)>;
		constexpr NStr::CStr gc_Bold = gc_Str<DMibCommandLineAnsiColor_Bold>;
		constexpr NStr::CStr gc_Italic = gc_Str<"\x1B[3m">;
		constexpr NStr::CStr gc_NotBold = gc_Str<"\x1B[22m">;
		constexpr NStr::CStr gc_NotItalic = gc_Str<"\x1B[23m">;

		constexpr NStr::CStr gc_Prompt = gc_Str<DMibCommandLineAnsiColor_Reset DMibCommandLineAnsiColor_256(221)>;

		uint8 fg_FindColor256(uint8 _Red, uint8 _Green, uint8 _Blue)
		{
			uint8 BestColor = 0;
			uint32 SmallestError = TCLimitsInt<uint32>::mc_Max;

			constexpr static uint8 c_CubeColors[6] = {0, 95, 135, 175, 215, 255};

			auto fBestComponent = [](uint8 _Value)
				{
					uint8 BestColor = 0;
					uint32 SmallestError = TCLimitsInt<uint32>::mc_Max;
					for (umint i = 0; i < 6; ++i)
					{
						uint32 Error = fg_Abs(int32(_Value) - int32(c_CubeColors[i]));
						Error = Error * Error;
						if (Error < SmallestError)
						{
							SmallestError = Error;
							BestColor = i;
						}
					}
					return BestColor;
				}
			;

			auto fError = [&](CAnsiEncodingParse::CDecodedColor const &_Color)
				{
					uint32 ErrorRed = fg_Abs(int32(_Red) - int32(_Color.m_Red));
					uint32 ErrorGreen = fg_Abs(int32(_Green) - int32(_Color.m_Green));
					uint32 ErrorBlue = fg_Abs(int32(_Blue) - int32(_Color.m_Blue));
					return ErrorRed * ErrorRed + ErrorGreen * ErrorGreen + ErrorBlue * ErrorBlue;
				}
			;

			{
				auto BestRed = fBestComponent(_Red);
				auto BestGreen = fBestComponent(_Green);
				auto BestBlue = fBestComponent(_Blue);
				BestColor = 16 + BestRed * 36 + BestGreen * 6 + BestBlue;
				SmallestError = fError(g_CommandLine_AnsiEncodingColor256Array[BestColor]);
			}

			for (umint i = 232; i < 256; ++i)
			{
				auto &Color = g_CommandLine_AnsiEncodingColor256Array[i];

				uint32 Error = fError(Color);
				if (Error < SmallestError)
				{
					SmallestError = Error;
					BestColor = i;
				}
			}

			return BestColor;
		}

		NGraphics::CColorR8G8B8 fg_InvertBrightness(uint8 _Red, uint8 _Green, uint8 _Blue)
		{
			fp32 R = fp32(1.0f/255.0f) * _Red;
			fp32 G = fp32(1.0f/255.0f) * _Green;
			fp32 B = fp32(1.0f/255.0f) * _Blue;

			fp32 Y = fp32(0.299f) * R + fp32(0.587f) * G + fp32(0.114f) * B;
			fp32 Cb = fp32(-0.1687f) * R - fp32(0.3313f) * G + fp32(0.5f) * B;
			fp32 Cr = fp32(0.5f) * R - fp32(0.4187f) * G - fp32(0.0813f) * B;

			Y = (fp32(1.0f) - Y.f_Pow(1.0f/1.5f));

			fp32 Rf = (Y + fp32(1.402f) * Cr);
			fp32 Gf = (Y - fp32(0.34414f) * Cb - fp32(0.71414f) * Cr);
			fp32 Bf = (Y + fp32(1.772f) * Cb);

			Rf *= 255.0f;
			Gf *= 255.0f;
			Bf *= 255.0f;
			Rf = fg_Clamp(Rf, fp32(0.0f), fp32(255.0f));
			Gf = fg_Clamp(Gf, fp32(0.0f), fp32(255.0f));
			Bf = fg_Clamp(Bf, fp32(0.0f), fp32(255.0f));

			return NGraphics::CColorR8G8B8::fs_FromRGB(Rf.f_ToIntRound(), Gf.f_ToIntRound(), Bf.f_ToIntRound());
		}
	}

	CAnsiEncoding::CAnsiEncoding(EAnsiEncodingFlag _Flags)
		: mp_Flags(_Flags)
	{
		if (mp_Flags & EAnsiEncodingFlag_ColorLightBackground)
		{
			mp_StatusNormal = DMibCommandLineAnsiColor_Reset + f_Foreground256(118);
			mp_StatusWarning = DMibCommandLineAnsiColor_Reset + f_Foreground256(207);
			mp_StatusError = DMibCommandLineAnsiColor_Reset DMibCommandLineAnsiColor_Bold + f_Foreground256(198);
			mp_Prompt = DMibCommandLineAnsiColor_Reset + f_Foreground256(221);
		}
	}

	NStr::CStr CAnsiEncoding::f_Foreground16(uint8 _Color) const
	{
		if (!(mp_Flags & EAnsiEncodingFlag_Color))
			return {};

		if (mp_Flags & (EAnsiEncodingFlag_Color24Bit | EAnsiEncodingFlag_ColorLightBackground))
		{
			CAnsiEncodingParse::CDecodedColor Color;
			Color.f_SetAnsi16(_Color);
			return f_ForegroundRGB(Color.m_Red, Color.m_Green, Color.m_Blue);
		}

		DMibRequire(_Color < 16);
		if (_Color < 8)
			_Color = 30 + _Color;
		else
			_Color = 90 + _Color;

		return "\x1B[{}m"_f << _Color;
	}

	NStr::CStr CAnsiEncoding::f_Background16(uint8 _Color) const
	{
		if (!(mp_Flags & EAnsiEncodingFlag_Color))
			return {};

		if (mp_Flags & (EAnsiEncodingFlag_Color24Bit | EAnsiEncodingFlag_ColorLightBackground))
		{
			CAnsiEncodingParse::CDecodedColor Color;
			Color.f_SetAnsi16(_Color);
			return f_BackgroundRGB(Color.m_Red, Color.m_Green, Color.m_Blue);
		}

		DMibRequire(_Color < 16);
		if (_Color < 8)
			_Color = 30 + _Color;
		else
			_Color = 90 + _Color;

		return "\x1B[{}m"_f << _Color;
	}

	NStr::CStr CAnsiEncoding::f_Foreground256(uint8 _Color) const
	{
		if (!(mp_Flags & EAnsiEncodingFlag_Color))
			return {};

		if (mp_Flags & (EAnsiEncodingFlag_Color24Bit | EAnsiEncodingFlag_ColorLightBackground))
		{
			CAnsiEncodingParse::CDecodedColor Color;
			Color.f_SetAnsi256(_Color);
			return f_ForegroundRGB(Color.m_Red, Color.m_Green, Color.m_Blue);
		}

		if (_Color < 16)
		{
			CAnsiEncodingParse::CDecodedColor Color;
			Color.f_SetAnsi16(_Color);
			_Color = fg_FindColor256(Color.m_Red, Color.m_Green, Color.m_Blue);
		}

		if (mp_Flags & EAnsiEncodingFlag_ColorSgrUsesSemiColon)
			return "\x1B[38;5;{}m"_f << _Color;
		else
			return "\x1B[38:5:{}m"_f << _Color;
	}

	NStr::CStr CAnsiEncoding::f_Background256(uint8 _Color) const
	{
		if (!(mp_Flags & EAnsiEncodingFlag_Color))
			return {};

		if (mp_Flags & (EAnsiEncodingFlag_Color24Bit | EAnsiEncodingFlag_ColorLightBackground))
		{
			CAnsiEncodingParse::CDecodedColor Color;
			Color.f_SetAnsi256(_Color);
			return f_BackgroundRGB(Color.m_Red, Color.m_Green, Color.m_Blue);
		}

		if (_Color < 16)
		{
			CAnsiEncodingParse::CDecodedColor Color;
			Color.f_SetAnsi16(_Color);
			_Color = fg_FindColor256(Color.m_Red, Color.m_Green, Color.m_Blue);
		}

		if (mp_Flags & EAnsiEncodingFlag_ColorSgrUsesSemiColon)
			return "\x1B[48;5;{}m"_f << _Color;
		else
			return "\x1B[48:5:{}m"_f << _Color;
	}

	NStr::CStr::CFormat CAnsiEncoding::f_ForegroundRGBFormat(uint32 _RGB) const
	{
		NGraphics::CColorR8G8B8 Color{.m_Color = _RGB};
		return f_ForegroundRGBFormat(Color.f_Red(), Color.f_Green(), Color.f_Blue());
	}

	NStr::CStr::CFormat CAnsiEncoding::f_ForegroundRGBFormat(uint8 _Red, uint8 _Green, uint8 _Blue) const
	{
		if (!(mp_Flags & EAnsiEncodingFlag_Color))
			return NStr::CStr::CFormat("");

		if (mp_Flags & EAnsiEncodingFlag_ColorLightBackground)
		{
			NGraphics::CColorR8G8B8 Inverted = fg_InvertBrightness(_Red, _Green, _Blue);
			_Red = Inverted.f_Red();
			_Green = Inverted.f_Green();
			_Blue = Inverted.f_Blue();
		}

		if (mp_Flags & EAnsiEncodingFlag_ColorSgrUsesSemiColon)
		{
			if (!(mp_Flags & EAnsiEncodingFlag_Color24Bit))
				return "\x1B[38;5;{}m"_f << fg_ByValue(fg_FindColor256(_Red, _Green, _Blue));

			return "\x1B[38;2;{};{};{}m"_f << fg_ByValue(_Red) << fg_ByValue(_Green) << fg_ByValue(_Blue);
		}
		else
		{
			if (!(mp_Flags & EAnsiEncodingFlag_Color24Bit))
				return "\x1B[38:5:{}m"_f << fg_ByValue(fg_FindColor256(_Red, _Green, _Blue));

			return "\x1B[38:2::{}:{}:{}m"_f << fg_ByValue(_Red) << fg_ByValue(_Green) << fg_ByValue(_Blue);
		}
	}

	NStr::CStr::CFormat CAnsiEncoding::f_BackgroundRGBFormat(uint32 _RGB) const
	{
		NGraphics::CColorR8G8B8 Color{.m_Color = _RGB};
		return f_BackgroundRGBFormat(Color.f_Red(), Color.f_Green(), Color.f_Blue());
	}

	NStr::CStr::CFormat CAnsiEncoding::f_BackgroundRGBFormat(uint8 _Red, uint8 _Green, uint8 _Blue) const
	{
		if (!(mp_Flags & EAnsiEncodingFlag_Color))
			return NStr::CStr::CFormat("");

		if (mp_Flags & EAnsiEncodingFlag_ColorLightBackground)
		{
			NGraphics::CColorR8G8B8 Inverted = fg_InvertBrightness(_Red, _Green, _Blue);
			_Red = Inverted.f_Red();
			_Green = Inverted.f_Green();
			_Blue = Inverted.f_Blue();
		}

		if (mp_Flags & EAnsiEncodingFlag_ColorSgrUsesSemiColon)
		{
			if (!(mp_Flags & EAnsiEncodingFlag_Color24Bit))
				return "\x1B[48;5;{}m"_f << fg_ByValue(fg_FindColor256(_Red, _Green, _Blue));

			return "\x1B[48;2;{};{};{}m"_f << fg_ByValue(_Red) << fg_ByValue(_Green) << fg_ByValue(_Blue);
		}
		else
		{
			if (!(mp_Flags & EAnsiEncodingFlag_Color24Bit))
				return "\x1B[48:5:{}m"_f << fg_ByValue(fg_FindColor256(_Red, _Green, _Blue));

			return "\x1B[48:2::{}:{}:{}m"_f << fg_ByValue(_Red) << fg_ByValue(_Green) << fg_ByValue(_Blue);
		}
	}

	NStr::CStr::CFormat CAnsiEncoding::f_UnderlineRGBFormat(uint32 _RGB) const
	{
		NGraphics::CColorR8G8B8 Color{.m_Color = _RGB};
		return f_UnderlineRGBFormat(Color.f_Red(), Color.f_Green(), Color.f_Blue());
	}

	NStr::CStr::CFormat CAnsiEncoding::f_UnderlineRGBFormat(uint8 _Red, uint8 _Green, uint8 _Blue) const
	{
		if (!(mp_Flags & EAnsiEncodingFlag_Color))
			return NStr::CStr::CFormat("");

		if (mp_Flags & EAnsiEncodingFlag_ColorLightBackground)
		{
			NGraphics::CColorR8G8B8 Inverted = fg_InvertBrightness(_Red, _Green, _Blue);
			_Red = Inverted.f_Red();
			_Green = Inverted.f_Green();
			_Blue = Inverted.f_Blue();
		}

		if (mp_Flags & EAnsiEncodingFlag_ColorSgrUsesSemiColon)
		{
			if (!(mp_Flags & EAnsiEncodingFlag_Color24Bit))
				return "\x1B[58;5;{}m"_f << fg_ByValue(fg_FindColor256(_Red, _Green, _Blue));

			return "\x1B[58;2;{};{};{}m"_f << fg_ByValue(_Red) << fg_ByValue(_Green) << fg_ByValue(_Blue);
		}
		else
		{
			if (!(mp_Flags & EAnsiEncodingFlag_Color24Bit))
				return "\x1B[58:5:{}m"_f << fg_ByValue(fg_FindColor256(_Red, _Green, _Blue));

			return "\x1B[58:2::{}:{}:{}m"_f << fg_ByValue(_Red) << fg_ByValue(_Green) << fg_ByValue(_Blue);
		}
	}

	NStr::CStr CAnsiEncoding::f_ForegroundRGB(uint32 _RGB) const
	{
		NGraphics::CColorR8G8B8 Color{.m_Color = _RGB};
		return f_ForegroundRGB(Color.f_Red(), Color.f_Green(), Color.f_Blue());
	}

	NStr::CStr CAnsiEncoding::f_ForegroundRGB(uint8 _Red, uint8 _Green, uint8 _Blue) const
	{
		if (!(mp_Flags & EAnsiEncodingFlag_Color))
			return {};

		if (mp_Flags & EAnsiEncodingFlag_ColorLightBackground)
		{
			NGraphics::CColorR8G8B8 Inverted = fg_InvertBrightness(_Red, _Green, _Blue);
			_Red = Inverted.f_Red();
			_Green = Inverted.f_Green();
			_Blue = Inverted.f_Blue();
		}

		if (mp_Flags & EAnsiEncodingFlag_ColorSgrUsesSemiColon)
		{
			if (!(mp_Flags & EAnsiEncodingFlag_Color24Bit))
				return "\x1B[38;5;{}m"_f << fg_FindColor256(_Red, _Green, _Blue);

			return "\x1B[38;2;{};{};{}m"_f << _Red << _Green << _Blue;
		}
		else
		{
			if (!(mp_Flags & EAnsiEncodingFlag_Color24Bit))
				return "\x1B[38:5:{}m"_f << fg_FindColor256(_Red, _Green, _Blue);

			return "\x1B[38:2::{}:{}:{}m"_f << _Red << _Green << _Blue;
		}
	}

	NStr::CStr CAnsiEncoding::f_BackgroundRGB(uint32 _RGB) const
	{
		NGraphics::CColorR8G8B8 Color{.m_Color = _RGB};
		return f_BackgroundRGB(Color.f_Red(), Color.f_Green(), Color.f_Blue());
	}

	NStr::CStr CAnsiEncoding::f_BackgroundRGB(uint8 _Red, uint8 _Green, uint8 _Blue) const
	{
		if (!(mp_Flags & EAnsiEncodingFlag_Color))
			return {};

		if (mp_Flags & EAnsiEncodingFlag_ColorLightBackground)
		{
			NGraphics::CColorR8G8B8 Inverted = fg_InvertBrightness(_Red, _Green, _Blue);
			_Red = Inverted.f_Red();
			_Green = Inverted.f_Green();
			_Blue = Inverted.f_Blue();
		}

		if (mp_Flags & EAnsiEncodingFlag_ColorSgrUsesSemiColon)
		{
			if (!(mp_Flags & EAnsiEncodingFlag_Color24Bit))
				return "\x1B[48;5;{}m"_f << fg_FindColor256(_Red, _Green, _Blue);

			return "\x1B[48;2;{};{};{}m"_f << _Red << _Green << _Blue;
		}
		else
		{
			if (!(mp_Flags & EAnsiEncodingFlag_Color24Bit))
				return "\x1B[48:5:{}m"_f << fg_FindColor256(_Red, _Green, _Blue);

			return "\x1B[48:2::{}:{}:{}m"_f << _Red << _Green << _Blue;
		}
	}

	NStr::CStr const &CAnsiEncoding::f_Default() const
	{
		if (!(mp_Flags & EAnsiEncodingFlag_Color))
			return gc_Empty;

		return gc_Default;
	}

	NStr::CStr const &CAnsiEncoding::f_Reset() const
	{
		return gc_Reset;
	}

	NStr::CStr CAnsiEncoding::f_SyncronizeOutputStart() const
	{
		return gc_Str<"\x1B[?2026h">;
	}

	NStr::CStr CAnsiEncoding::f_SyncronizeOutputFinish() const
	{
		return gc_Str<"\x1B[?2026l">;
	}

	NStr::CFStr24 CAnsiEncoding::f_ShowCursor(bool _bShow) const
	{
		return NStr::CFStr24::CFormat("\x1B[?25{}") << (_bShow ? "h" : "l");
	}

	NStr::CFStr24 CAnsiEncoding::f_EnableAlternativeScreenBuffer(bool _bEnable) const
	{
		return NStr::CFStr24::CFormat("\x1B[?1049{}") << (_bEnable ? "h" : "l");
	}

	NStr::CFStr24 CAnsiEncoding::f_EnableMouseReporting(bool _bEnable) const
	{
		// Button-event tracking with SGR extended coordinates
		return NStr::CFStr24::CFormat("\x1B[?1002{}\x1B[?1006{}") << (_bEnable ? "h" : "l") << (_bEnable ? "h" : "l");
	}

	NStr::CFStr24 CAnsiEncoding::f_PushComprehensiveKeyHandling(EComprehensiveKeyFlags _KeyHandlingFlags) const
	{
		return NStr::CFStr24::CFormat("\x1B[>{}u") << uint8(_KeyHandlingFlags);
	}

	NStr::CStr const &CAnsiEncoding::f_PopComprehensiveKeyHandling() const
	{
		return gc_Str<"\x1B[<u">;
	}

	NStr::CStr const &CAnsiEncoding::f_MoveUpperLeft() const
	{
		return gc_UpperLeft;
	}

	NStr::CFStr24 CAnsiEncoding::f_Move(uint32 _Row, uint32 _Column) const
	{
		return NStr::CFStr24::CFormat("\x1B[{};{}H") << (_Row + 1) << (_Column + 1);
	}

	NStr::CStr::CFormat CAnsiEncoding::f_MoveToColumn(uint32 _Column) const
	{
		return "\x1B[{}G"_f << fg_ByValue(_Column + 1);
	}

	NStr::CStr::CFormat CAnsiEncoding::f_MovePreviousLine(uint32 _nLines) const
	{
		if (_nLines == 1)
			return "\x1B[A"_f;
		else
			return "\x1B[{}A"_f << fg_ByValue(_nLines);
	}

	NStr::CFStr24 CAnsiEncoding::f_MoveNextLine(uint32 _nLines) const
	{
		if (_nLines == 1)
			return "\x1B[E";
		else
			return NStr::CFStr24::CFormat("\x1B[{}E") << _nLines;
	}

	NStr::CStr const &CAnsiEncoding::f_ClearToEndOfScreen() const
	{
		return gc_ClearToEndOfScreen;
	}

	NStr::CStr const &CAnsiEncoding::f_StatusNormal() const
	{
		if (!(mp_Flags & EAnsiEncodingFlag_Color))
			return gc_Empty;
		if (mp_Flags & EAnsiEncodingFlag_ColorLightBackground)
			return mp_StatusNormal;
		return gc_StatusNormal;
	}

	NStr::CStr const &CAnsiEncoding::f_StatusWarning() const
	{
		if (!(mp_Flags & EAnsiEncodingFlag_Color))
			return gc_Empty;
		if (mp_Flags & EAnsiEncodingFlag_ColorLightBackground)
			return mp_StatusWarning;
		return gc_StatusWarning;
	}

	NStr::CStr const &CAnsiEncoding::f_StatusError() const
	{
		if (!(mp_Flags & EAnsiEncodingFlag_Color))
			return gc_Empty;
		if (mp_Flags & EAnsiEncodingFlag_ColorLightBackground)
			return mp_StatusError;
		return gc_StatusError;
	}

	NStr::CStr const &CAnsiEncoding::f_Weight(EWeight _Weight) const
	{
		if (!(mp_Flags & EAnsiEncodingFlag_Color))
			return gc_Empty;

		switch (_Weight)
		{
		case EWeight::mc_Normal: return gc_NotBold;
		case EWeight::mc_Bold: return gc_Bold;
		case EWeight::mc_Dim: return gc_Str<"\x1B[2m">;
		case EWeight::mc_Shadowed: return gc_Str<"\x1B[1:2m">;
		}

		return gc_Empty;
	}

	NStr::CStr const &CAnsiEncoding::f_Underline(EUnderline _Underline) const
	{
		if (!(mp_Flags & EAnsiEncodingFlag_Color))
			return gc_Empty;

		switch (_Underline)
		{
		case EUnderline::mc_None: return gc_Str<"\x1B[24m">;
		case EUnderline::mc_Solid: return gc_Str<"\x1B[4m">;
		case EUnderline::mc_Double: return gc_Str<"\x1B[4:2m">;
		case EUnderline::mc_Wavy: return gc_Str<"\x1B[4:3m">;
		case EUnderline::mc_Dotted: return gc_Str<"\x1B[4:4m">;
		case EUnderline::mc_Dashed: return gc_Str<"\x1B[4:5m">;
		}

		return gc_Empty;
	}

	NStr::CStr const &CAnsiEncoding::f_Bold() const
	{
		if (!(mp_Flags & EAnsiEncodingFlag_Color))
			return gc_Empty;

		return gc_Bold;
	}

	NStr::CStr const &CAnsiEncoding::f_NotBold() const
	{
		if (!(mp_Flags & EAnsiEncodingFlag_Color))
			return gc_Empty;

		return gc_NotBold;
	}

	NStr::CStr const &CAnsiEncoding::f_Italic() const
	{
		if (!(mp_Flags & EAnsiEncodingFlag_Color))
			return gc_Empty;

		return gc_Italic;
	}

	NStr::CStr const &CAnsiEncoding::f_NotItalic() const
	{
		if (!(mp_Flags & EAnsiEncodingFlag_Color))
			return gc_Empty;

		return gc_NotItalic;
	}

	NStr::CStr const &CAnsiEncoding::f_Strikeout() const
	{
		if (!(mp_Flags & EAnsiEncodingFlag_Color))
			return gc_Empty;

		return gc_Str<"\x1B[9m">;
	}

	NStr::CStr const &CAnsiEncoding::f_NotStrikeout() const
	{
		if (!(mp_Flags & EAnsiEncodingFlag_Color))
			return gc_Empty;

		return gc_Str<"\x1B[29m">;
	}

	EAnsiEncodingFlag CAnsiEncoding::f_Flags() const
	{
		return mp_Flags;
	}

	NStr::CStr const &CAnsiEncoding::f_Prompt() const
	{
		if (!(mp_Flags & EAnsiEncodingFlag_Color))
			return gc_Empty;
		if (mp_Flags & EAnsiEncodingFlag_ColorLightBackground)
			return mp_Prompt;
		return gc_Prompt;
	}

	NStr::CStr CAnsiEncoding::f_StatusNormal(NStr::CStr const &_ToWrap) const
	{
		return "{}{}{}"_f << f_StatusNormal() << _ToWrap << f_Default();
	}

	NStr::CStr CAnsiEncoding::f_StatusWarning(NStr::CStr const &_ToWrap) const
	{
		return "{}{}{}"_f << f_StatusWarning() << _ToWrap << f_Default();
	}

	NStr::CStr CAnsiEncoding::f_StatusError(NStr::CStr const &_ToWrap) const
	{
		return "{}{}{}"_f << f_StatusError() << _ToWrap << f_Default();
	}

	NStr::CStr CAnsiEncoding::f_Bold(NStr::CStr const &_ToWrap) const
	{
		return "{}{}{}"_f << f_Bold() << _ToWrap << f_Default();
	}

	NStr::CStr CAnsiEncoding::f_Prompt(NStr::CStr const &_ToWrap) const
	{
		return "{}{}{}"_f << f_Prompt() << _ToWrap << f_Default();
	}

	bool CAnsiEncoding::f_Color() const
	{
		return mp_Flags & EAnsiEncodingFlag_Color;
	}

	CStr CAnsiEncoding::f_ReEncode(CStr const &_In) const
	{
		CStr Ret;

		CAnsiEncodingParse::fs_Parse
			(
				_In
				, [&](CStrPtr const &_String) -> bool
				{
					Ret += _String;

					return true;
				}
				, [&](CAnsiEncodingParse::CPropertyChange const &_Change)
				{
					if (!f_Color())
						return;

					_Change.f_Visit
						(
							[&]<typename tf_CType>(tf_CType const &_Change)
							{
								if constexpr (NTraits::cIsSame<tf_CType, CAnsiEncodingParse::CReset>)
									Ret += f_Default();
								else if constexpr (NTraits::cIsSame<tf_CType, CAnsiEncodingParse::CWeight>)
									Ret += f_Weight(_Change.m_Weight);
								else if constexpr (NTraits::cIsSame<tf_CType, CAnsiEncodingParse::CUnderline>)
									Ret += f_Underline(_Change.m_Underline);
								else if constexpr (NTraits::cIsSame<tf_CType, CAnsiEncodingParse::CItalic>)
								{
									if (_Change.m_bEnabled)
										Ret += f_Italic();
									else
										Ret += f_NotItalic();
								}
								else if constexpr (NTraits::cIsSame<tf_CType, CAnsiEncodingParse::CStrikeout>)
								{
									if (_Change.m_bEnabled)
										Ret += f_Strikeout();
									else
										Ret += f_NotStrikeout();
								}
								else if constexpr (NTraits::cIsSame<tf_CType, CAnsiEncodingParse::CForegroundColor>)
								{
									auto &Color = _Change;
									Ret += f_ForegroundRGBFormat(Color.m_Red, Color.m_Green, Color.m_Blue);
								}
								else if constexpr (NTraits::cIsSame<tf_CType, CAnsiEncodingParse::CBackgroundColor>)
								{
									auto &Color = _Change;
									Ret += f_BackgroundRGBFormat(Color.m_Red, Color.m_Green, Color.m_Blue);
								}
								else if constexpr (NTraits::cIsSame<tf_CType, CAnsiEncodingParse::CUnderlineColor>)
								{
									auto &Color = _Change;
									Ret += f_UnderlineRGBFormat(Color.m_Red, Color.m_Green, Color.m_Blue);
								}
								else
									static_assert(false);
							}
						)
					;
				}
			)
		;
		return Ret;
	}

	TCVector<CAnsiEncoding::CLine> CAnsiEncoding::f_LineBreak(CStr const &_String, umint _Length, EWordWrap _WordWrap) const
	{
		TCVector<CLine> Output;
		f_LineBreak(_String, _Length, _WordWrap, Output);

		return Output;
	}

	namespace
	{
		// Decodes the UTF-8 codepoint at _pParse and advances past it. The buffer is NUL
		// terminated; invalid or truncated sequences decode as their first byte so every input
		// byte round-trips unchanged into the output lines.
		ch32 fg_DecodeUTF8Char(uch8 const *&_pParse)
		{
			uch8 First = *_pParse;
			++_pParse;

			if (First < 0x80)
				return First;

			umint nContinuations;
			ch32 Char;
			if ((First & 0xE0) == 0xC0)
			{
				nContinuations = 1;
				Char = First & 0x1F;
			}
			else if ((First & 0xF0) == 0xE0)
			{
				nContinuations = 2;
				Char = First & 0x0F;
			}
			else if ((First & 0xF8) == 0xF0)
			{
				nContinuations = 3;
				Char = First & 0x07;
			}
			else
				return First;

			uch8 const *pCheck = _pParse;
			for (umint iContinuation = 0; iContinuation < nContinuations; ++iContinuation)
			{
				uch8 Byte = *pCheck;
				if ((Byte & 0xC0) != 0x80)
					return First;

				Char = (Char << 6) | (Byte & 0x3F);
				++pCheck;
			}

			_pParse = pCheck;

			return Char;
		}

		ch32 fg_PeekUTF8Char(uch8 const *_pParse)
		{
			return fg_DecodeUTF8Char(_pParse);
		}
	}

	namespace
	{
		struct CLineBreakPropertyPoint
		{
			umint m_Offset;
			CAnsiEncodingParse::CActiveProperties m_Properties;
		};

		// First phase of line breaking: strips escape sequences and records the active properties
		// at each stripped-string byte offset where they change. Plain input produces no property
		// points, and its stripped string shares the input's storage, so this phase allocates
		// nothing for text without escape sequences.
		void fg_LineBreakParse(CStr const &_String, CStr &o_Stripped, TCVector<CLineBreakPropertyPoint> &o_Points)
		{
			CAnsiEncodingParse::CActiveProperties CurrentProperties;

			CAnsiEncodingParse::fs_Parse
				(
					_String
					, [&](CStrPtr const &_Text) -> bool
					{
						// A run covering the whole input means there were no escape sequences;
						// share the input's storage instead of copying the bytes
						if
						(
							o_Stripped.f_IsEmpty()
							&& _Text.f_GetStr() == _String.f_GetStr()
							&& _Text.f_GetLen() == _String.f_GetLen()
						)
						{
							o_Stripped = _String;
						}
						else
							o_Stripped += _Text;

						return true;
					}
					, [&](CAnsiEncodingParse::CPropertyChange const &_Change)
					{
						_Change.f_Visit
							(
								[&]<typename tf_CType>(tf_CType const &_Change)
								{
									if constexpr (NTraits::cIsSame<tf_CType, CAnsiEncodingParse::CReset>)
									{
										CurrentProperties.m_BackgroundColor.f_Clear();
										CurrentProperties.m_ForegroundColor.f_Clear();
										CurrentProperties.m_UnderlineColor.f_Clear();
										CurrentProperties.m_Weight.f_Clear();
										CurrentProperties.m_Underline.f_Clear();
										CurrentProperties.m_Italic.f_Clear();
										CurrentProperties.m_Strikeout.f_Clear();
									}
									else if constexpr (NTraits::cIsSame<tf_CType, CAnsiEncodingParse::CWeight>)
									{
										auto Weight = _Change.m_Weight;
										if (Weight == CAnsiEncoding::EWeight::mc_Normal)
											CurrentProperties.m_Weight.f_Clear();
										else
											CurrentProperties.m_Weight = {Weight};
									}
									else if constexpr (NTraits::cIsSame<tf_CType, CAnsiEncodingParse::CUnderline>)
									{
										auto Underline = _Change.m_Underline;
										if (Underline == CAnsiEncoding::EUnderline::mc_None)
											CurrentProperties.m_Underline.f_Clear();
										else
											CurrentProperties.m_Underline = {Underline};
									}
									else if constexpr (NTraits::cIsSame<tf_CType, CAnsiEncodingParse::CItalic>)
									{
										if (_Change.m_bEnabled)
											CurrentProperties.m_Italic = {true};
										else
											CurrentProperties.m_Italic.f_Clear();
									}
									else if constexpr (NTraits::cIsSame<tf_CType, CAnsiEncodingParse::CStrikeout>)
									{
										if (_Change.m_bEnabled)
											CurrentProperties.m_Strikeout = {true};
										else
											CurrentProperties.m_Strikeout.f_Clear();
									}
									else if constexpr (NTraits::cIsSame<tf_CType, CAnsiEncodingParse::CForegroundColor>)
									{
										auto &Color = _Change;
										if (Color.m_bEnabled)
											CurrentProperties.m_ForegroundColor = Color;
										else
											CurrentProperties.m_ForegroundColor.f_Clear();
									}
									else if constexpr (NTraits::cIsSame<tf_CType, CAnsiEncodingParse::CBackgroundColor>)
									{
										auto &Color = _Change;
										if (Color.m_bEnabled)
											CurrentProperties.m_BackgroundColor = Color;
										else
											CurrentProperties.m_BackgroundColor.f_Clear();
									}
									else if constexpr (NTraits::cIsSame<tf_CType, CAnsiEncodingParse::CUnderlineColor>)
									{
										auto &Color = _Change;
										if (Color.m_bEnabled)
											CurrentProperties.m_UnderlineColor = Color;
										else
											CurrentProperties.m_UnderlineColor.f_Clear();
									}
									else
										static_assert(false);

									umint Offset = o_Stripped.f_GetLen();
									if (!o_Points.f_IsEmpty() && o_Points.f_GetLast().m_Offset == Offset)
										o_Points.f_GetLast().m_Properties = CurrentProperties;
									else
										o_Points.f_InsertLast({Offset, CurrentProperties});
								}
							)
						;
					}
				)
			;
		}

		// Second phase: the wrap state machine over the stripped UTF-8 bytes. Lines are reported
		// front to back through _fOutputLine(pStart, nBytes, DisplayLen, bLeadingEllipsis), and
		// _fAppendEllipsis(WidthDelta) appends a trailing "…" to the most recently output line.
		template <typename t_FOutputLine, typename t_FAppendEllipsis>
		void fg_LineBreakWalk
			(
				CStr const &_Stripped
				, umint _Length
				, CAnsiEncoding::EWordWrap _WordWrap
				, t_FOutputLine &&_fOutputLine
				, t_FAppendEllipsis &&_fAppendEllipsis
			)
		{
			ch8 const *pStrippedBase = _Stripped.f_GetStr();
			uch8 const *pParse = (uch8 const *)pStrippedBase;
			uch8 const *pParseStart = pParse;
			uch8 const *pLastWord = nullptr;

			umint Len = 0;
			umint MaxLen = _Length;

			bool bWasEllipsis = false;
			auto fOutputLine = [&](uch8 const *_pStart, umint _nBytes, umint _DisplayLen)
				{
					_fOutputLine(_pStart, _nBytes, _DisplayLen, bWasEllipsis);
					bWasEllipsis = false;
				}
			;

			bool bLastWasNewLine = true;
			bool bOnlyWhitespaceAfterNewLine = true;
			auto pLastDisplayPoint = pParse;
			umint LastWordLen = 0;
			umint LastDisplayPointLen = 0;

			while (*pParse)
			{
				ch32 Char = fg_DecodeUTF8Char(pParse);
				if (Char == '\r' || Char == '\n')
				{
					fOutputLine(pParseStart, (pParse - pParseStart) - 1, Len);

					if (Char == '\r' && *pParse == '\n')
						++pParse;

					bLastWasNewLine = true;
					pParseStart = pParse;
					pLastWord = nullptr;
					bOnlyWhitespaceAfterNewLine = true;
					Len = 0;
					continue;
				}
				else
				{
					bLastWasNewLine = false;
					if (!fg_CharIsWhiteSpaceNoLines(Char))
						bOnlyWhitespaceAfterNewLine = false;
				}

				++Len;
				while (fg_CharIsCombining(Char))
				{
					if (!*pParse)
					{
						Char = 0;
						break;
					}
					Char = fg_DecodeUTF8Char(pParse);
				}

				if (Len == MaxLen && *pParse)
				{
					if ((_WordWrap == CAnsiEncoding::EWordWrap_Word || _WordWrap == CAnsiEncoding::EWordWrap_WordEllipsis) && pLastWord)
					{
						if (pLastWord - pParseStart)
						{
							fOutputLine(pParseStart, pLastWord - pParseStart, LastWordLen);
							pParseStart = pLastWord;
						}
						else
						{
							fOutputLine(pParseStart, pParse - pParseStart, Len);
							pParseStart = pParse;
						}
						while (fg_CharIsWhiteSpaceNoLines(fg_PeekUTF8Char(pParseStart)))
							fg_DecodeUTF8Char(pParseStart);
						pParse = pParseStart;
					}
					else
					{
						if (_WordWrap == CAnsiEncoding::EWordWrap_WordEllipsis || _WordWrap == CAnsiEncoding::EWordWrap_CharacterEllipsis)
						{
							fOutputLine(pParseStart, pLastDisplayPoint - pParseStart, LastDisplayPointLen);
							if (!fg_CharIsWhiteSpaceNoLines(fg_PeekUTF8Char(pLastDisplayPoint)))
							{
								_fAppendEllipsis(1);
								bWasEllipsis = true;
								pParse = pParseStart = pLastDisplayPoint;
							}
							else
							{
								pParse = pLastDisplayPoint;
								while (fg_CharIsWhiteSpaceNoLines(fg_PeekUTF8Char(pParse)))
									fg_DecodeUTF8Char(pParse);
								pParseStart = pParse;
							}
						}
						else if (_WordWrap == CAnsiEncoding::EWordWrap_None)
							fOutputLine(pParseStart, pParse - pParseStart, Len);
						else if (_WordWrap == CAnsiEncoding::EWordWrap_Ellipsis)
						{
							fOutputLine(pParseStart, pLastDisplayPoint - pParseStart, LastDisplayPointLen + 1);
							_fAppendEllipsis(0);
						}
						else
						{
							fOutputLine(pParseStart, pParse - pParseStart, Len);
							pParseStart = pParse;
						}

						if (_WordWrap == CAnsiEncoding::EWordWrap_None || _WordWrap == CAnsiEncoding::EWordWrap_Ellipsis)
						{
							fg_ParseToEndOfLine(pParse);
							if (fg_CharIsNewLine(*pParse))
							{
								bLastWasNewLine = true;
								bOnlyWhitespaceAfterNewLine = true;
							}
							fg_ParseEndOfLine(pParse);
							pParseStart = pParse;
						}
					}

					pLastWord = nullptr;
					if (bWasEllipsis)
						Len = 1;
					else
						Len = 0;
				}

				if (fg_CharIsWhiteSpaceNoLines(Char) && !bOnlyWhitespaceAfterNewLine)
				{
					pLastWord = pParse;
					LastWordLen = Len;
				}
				pLastDisplayPoint = pParse;
				LastDisplayPointLen = Len;
			}

			if (pParseStart != pParse || bLastWasNewLine)
				fOutputLine(pParseStart, pParse - pParseStart, Len);
		}
	}

	void CAnsiEncoding::f_LineBreak(CStr const &_String, umint _Length, EWordWrap _WordWrap, TCVector<CLine> &o_Lines) const
	{
		DMibRequire(_Length > 0);
		DMibRequire((_WordWrap != EWordWrap_WordEllipsis && _WordWrap != EWordWrap_CharacterEllipsis) || _Length > 2);

		CStr Stripped;
		TCVector<CLineBreakPropertyPoint> PropertyPoints;
		fg_LineBreakParse(_String, Stripped, PropertyPoints);

		ch8 const *pStrippedBase = Stripped.f_GetStr();

		TCVector<CLine> &Output = o_Lines;
		Output.f_ClearNoTrim();

		// Lines are output front to back, so one forward cursor over the property points tracks
		// the properties active at each line start
		umint iNextPoint = 0;
		CAnsiEncodingParse::CActiveProperties ActiveProperties;

		fg_LineBreakWalk
			(
				Stripped
				, _Length
				, _WordWrap
				, [&](uch8 const *_pStart, umint _nBytes, umint _DisplayLen, bool _bLeadingEllipsis)
				{
					umint Position = umint((ch8 const *)_pStart - pStrippedBase);
					umint EndPos = Position + _nBytes;

					while (iNextPoint < PropertyPoints.f_GetLen() && PropertyPoints[iNextPoint].m_Offset <= Position)
					{
						ActiveProperties = PropertyPoints[iNextPoint].m_Properties;
						++iNextPoint;
					}

					CStr ToOutput;
					if (_bLeadingEllipsis)
						ToOutput = "…";

					bool bSetProperties = false;
					umint ChunkPos = Position;
					while (ChunkPos < EndPos)
					{
						if (bSetProperties)
						{
							ToOutput += f_Default();
							bSetProperties = false;
						}

						if (ActiveProperties.m_Weight)
						{
							ToOutput += f_Weight(ActiveProperties.m_Weight->m_Weight);
							bSetProperties = true;
						}

						if (ActiveProperties.m_Underline)
						{
							ToOutput += f_Underline(ActiveProperties.m_Underline->m_Underline);
							bSetProperties = true;
						}

						if (ActiveProperties.m_Italic && ActiveProperties.m_Italic->m_bEnabled)
						{
							ToOutput += f_Italic();
							bSetProperties = true;
						}

						if (ActiveProperties.m_Strikeout && ActiveProperties.m_Strikeout->m_bEnabled)
						{
							ToOutput += f_Strikeout();
							bSetProperties = true;
						}

						if (ActiveProperties.m_BackgroundColor && ActiveProperties.m_BackgroundColor->m_bEnabled)
						{
							ToOutput += f_BackgroundRGBFormat(ActiveProperties.m_BackgroundColor->m_Red, ActiveProperties.m_BackgroundColor->m_Green, ActiveProperties.m_BackgroundColor->m_Blue);
							bSetProperties = true;
						}

						if (ActiveProperties.m_ForegroundColor && ActiveProperties.m_ForegroundColor->m_bEnabled)
						{
							ToOutput += f_ForegroundRGBFormat(ActiveProperties.m_ForegroundColor->m_Red, ActiveProperties.m_ForegroundColor->m_Green, ActiveProperties.m_ForegroundColor->m_Blue);
							bSetProperties = true;
						}

						if (ActiveProperties.m_UnderlineColor && ActiveProperties.m_UnderlineColor->m_bEnabled)
						{
							ToOutput += f_UnderlineRGBFormat(ActiveProperties.m_UnderlineColor->m_Red, ActiveProperties.m_UnderlineColor->m_Green, ActiveProperties.m_UnderlineColor->m_Blue);
							bSetProperties = true;
						}

						umint ChunkEnd = EndPos;
						if (iNextPoint < PropertyPoints.f_GetLen())
							ChunkEnd = fg_Min(EndPos, PropertyPoints[iNextPoint].m_Offset);

						ToOutput.f_AddStr(pStrippedBase + ChunkPos, ChunkEnd - ChunkPos);
						ChunkPos = ChunkEnd;

						if (ChunkPos < EndPos)
						{
							ActiveProperties = PropertyPoints[iNextPoint].m_Properties;
							++iNextPoint;
						}
					}

					if (bSetProperties)
						ToOutput += f_Default();

					umint OriginalLen = ToOutput.f_GetLen();
					fg_StrTrimRight(ToOutput);
					umint DisplayLen = _DisplayLen - (OriginalLen - ToOutput.f_GetLen());
					Output.f_Insert({fg_Move(ToOutput), DisplayLen});
				}
				, [&](umint _WidthDelta)
				{
					auto &LastOutput = Output.f_GetLast();
					LastOutput.m_String += "…";
					LastOutput.m_Width += _WidthDelta;
				}
			)
		;
	}

	CStr CAnsiEncoding::f_SyntaxColor(ESyntaxColor _Color) const
	{
		if (!(mp_Flags & EAnsiEncodingFlag_Color))
			return {};

		switch (_Color)
		{
		case ESyntaxColor_String: return f_ForegroundRGB(0x00, 0x9e, 0xff);
		case ESyntaxColor_Number: return f_ForegroundRGB(0xff, 0x00, 0x80);
		case ESyntaxColor_Constant: return f_ForegroundRGB(0xff, 0x8a, 0xc5);
		case ESyntaxColor_Comment: return f_ForegroundRGB(0x89, 0x89, 0x89);
		case ESyntaxColor_MemberVariable: return f_ForegroundRGB(0xff, 0xa6, 0x00);
		default: return {};
		}
	}

	NStr::CStr CAnsiEncoding::f_CreateSemiUniqueColor(NStr::CStr const &_Data) const
	{
		NCryptography::CHash_SHA256 Hash;
		Hash.f_AddData(_Data.f_GetStr(), _Data.f_GetLen());
		auto Digest = Hash.f_GetDigest();

		auto Red = Digest.f_GetData()[0];
		auto Green = Digest.f_GetData()[1];
		auto Blue = Digest.f_GetData()[2];

		if ((Red + Green + Blue) < 50)
		{
			Red += 50;
			Green += 50;
			Blue += 50;
		}

		return f_ForegroundRGB(Red, Green, Blue);
	}

	CAnsiEncoding::CSgrSequence::CSgrSequence(NStr::CStr::CAppender *_pAppender, EAnsiEncodingFlag _Flags)
		: mp_pAppender(_pAppender)
		, mp_Flags(_Flags)
	{
	}

	CAnsiEncoding::CSgrSequence::~CSgrSequence()
	{
		if (!mp_bFirst)
			*mp_pAppender += 'm';
	}

	CAnsiEncoding::CSgrSequence CAnsiEncoding::f_StartSgr(NStr::CStr::CAppender &_Appender)
	{
		return CSgrSequence(&_Appender, mp_Flags);
	}

	void CAnsiEncoding::CSgrSequence::fp_StartSequence()
	{
		if (mp_bFirst)
		{
			*mp_pAppender += gc_Str<"\x1B[">.m_Str;
			mp_bFirst = false;
			return;
		}

		*mp_pAppender += ';';
	}

	void CAnsiEncoding::CSgrSequence::f_ForegroundRGB(uint8 _Red, uint8 _Green, uint8 _Blue)
	{
		if (!(mp_Flags & EAnsiEncodingFlag_Color))
			return;

		if (mp_Flags & EAnsiEncodingFlag_ColorLightBackground)
		{
			NGraphics::CColorR8G8B8 Inverted = fg_InvertBrightness(_Red, _Green, _Blue);
			_Red = Inverted.f_Red();
			_Green = Inverted.f_Green();
			_Blue = Inverted.f_Blue();
		}

		fp_StartSequence();
		
		if (mp_Flags & EAnsiEncodingFlag_ColorSgrUsesSemiColon)
		{
			if (mp_Flags & EAnsiEncodingFlag_Color24Bit)
			{
				CFStr24 Format = CFStr24::CFormat("38;2;{};{};{}") << _Red << _Green << _Blue;
				*mp_pAppender += Format;
				return;
			}

			auto Color = fg_FindColor256(_Red, _Green, _Blue);
			CFStr24 Format = CFStr24::CFormat("38;5;{}") << Color;
			*mp_pAppender += Format;
		}
		else
		{
			if (mp_Flags & EAnsiEncodingFlag_Color24Bit)
			{
				CFStr24 Format = CFStr24::CFormat("38:2::{}:{}:{}") << _Red << _Green << _Blue;
				*mp_pAppender += Format;
				return;
			}

			auto Color = fg_FindColor256(_Red, _Green, _Blue);
			CFStr24 Format = CFStr24::CFormat("38:5:{}") << Color;
			*mp_pAppender += Format;
		}
	}

	void CAnsiEncoding::CSgrSequence::f_BackgroundRGB(uint8 _Red, uint8 _Green, uint8 _Blue)
	{
		if (!(mp_Flags & EAnsiEncodingFlag_Color))
			return;

		if (mp_Flags & EAnsiEncodingFlag_ColorLightBackground)
		{
			NGraphics::CColorR8G8B8 Inverted = fg_InvertBrightness(_Red, _Green, _Blue);
			_Red = Inverted.f_Red();
			_Green = Inverted.f_Green();
			_Blue = Inverted.f_Blue();
		}

		fp_StartSequence();
		
		if (mp_Flags & EAnsiEncodingFlag_ColorSgrUsesSemiColon)
		{
			if (mp_Flags & EAnsiEncodingFlag_Color24Bit)
			{
				CFStr24 Format = CFStr24::CFormat("48;2;{};{};{}") << _Red << _Green << _Blue;
				*mp_pAppender += Format;
				return;
			}

			auto Color = fg_FindColor256(_Red, _Green, _Blue);
			CFStr24 Format = CFStr24::CFormat("48;5;{}") << Color;
			*mp_pAppender += Format;
		}
		else
		{
			if (mp_Flags & EAnsiEncodingFlag_Color24Bit)
			{
				CFStr24 Format = CFStr24::CFormat("48:2::{}:{}:{}") << _Red << _Green << _Blue;
				*mp_pAppender += Format;
				return;
			}

			auto Color = fg_FindColor256(_Red, _Green, _Blue);
			CFStr24 Format = CFStr24::CFormat("48:5:{}") << Color;
			*mp_pAppender += Format;
		}
	}

	void CAnsiEncoding::CSgrSequence::f_UnderlineRGB(uint8 _Red, uint8 _Green, uint8 _Blue)
	{
		if (!(mp_Flags & EAnsiEncodingFlag_Color))
			return;

		if (mp_Flags & EAnsiEncodingFlag_ColorLightBackground)
		{
			NGraphics::CColorR8G8B8 Inverted = fg_InvertBrightness(_Red, _Green, _Blue);
			_Red = Inverted.f_Red();
			_Green = Inverted.f_Green();
			_Blue = Inverted.f_Blue();
		}

		fp_StartSequence();

		if (mp_Flags & EAnsiEncodingFlag_ColorSgrUsesSemiColon)
		{
			if (mp_Flags & EAnsiEncodingFlag_Color24Bit)
			{
				CFStr24 Format = CFStr24::CFormat("58;2;{};{};{}") << _Red << _Green << _Blue;
				*mp_pAppender += Format;
				return;
			}

			auto Color = fg_FindColor256(_Red, _Green, _Blue);
			CFStr24 Format = CFStr24::CFormat("58;5;{}") << Color;
			*mp_pAppender += Format;
		}
		else
		{
			if (mp_Flags & EAnsiEncodingFlag_Color24Bit)
			{
				CFStr24 Format = CFStr24::CFormat("58:2::{}:{}:{}") << _Red << _Green << _Blue;
				*mp_pAppender += Format;
				return;
			}

			auto Color = fg_FindColor256(_Red, _Green, _Blue);
			CFStr24 Format = CFStr24::CFormat("58:5:{}") << Color;
			*mp_pAppender += Format;
		}
	}

	uint32 CAnsiEncoding::CSgrSequence::f_ForegroundRGBDiff(uint32 _Previous, uint8 _Red, uint8 _Green, uint8 _Blue)
	{
		if (!(mp_Flags & EAnsiEncodingFlag_Color))
			return 0;

		if (mp_Flags & EAnsiEncodingFlag_ColorLightBackground)
		{
			NGraphics::CColorR8G8B8 Inverted = fg_InvertBrightness(_Red, _Green, _Blue);
			_Red = Inverted.f_Red();
			_Green = Inverted.f_Green();
			_Blue = Inverted.f_Blue();
		}

		if (!(mp_Flags & EAnsiEncodingFlag_ColorSgrUsesSemiColon))
		{
			if (mp_Flags & EAnsiEncodingFlag_Color24Bit)
			{
				fp_StartSequence();
				CFStr24 Format = CFStr24::CFormat("38:2::{}:{}:{}") << _Red << _Green << _Blue;
				*mp_pAppender += Format;
				return 0;
			}

			auto Color = fg_FindColor256(_Red, _Green, _Blue);
			if (Color == _Previous)
				return Color;

			fp_StartSequence();
			CFStr24 Format = CFStr24::CFormat("38:5:{}") << Color;
			*mp_pAppender += Format;
			return Color;
		}
		else
		{
			if (mp_Flags & EAnsiEncodingFlag_Color24Bit)
			{
				fp_StartSequence();
				CFStr24 Format = CFStr24::CFormat("38;2;{};{};{}") << _Red << _Green << _Blue;
				*mp_pAppender += Format;
				return 0;
			}

			auto Color = fg_FindColor256(_Red, _Green, _Blue);
			if (Color == _Previous)
				return Color;

			fp_StartSequence();
			CFStr24 Format = CFStr24::CFormat("38;5;{}") << Color;
			*mp_pAppender += Format;
			return Color;
		}
	}

	uint32 CAnsiEncoding::CSgrSequence::f_BackgroundRGBDiff(uint32 _Previous, uint8 _Red, uint8 _Green, uint8 _Blue)
	{
		if (!(mp_Flags & EAnsiEncodingFlag_Color))
			return 0;

		if (mp_Flags & EAnsiEncodingFlag_ColorLightBackground)
		{
			NGraphics::CColorR8G8B8 Inverted = fg_InvertBrightness(_Red, _Green, _Blue);
			_Red = Inverted.f_Red();
			_Green = Inverted.f_Green();
			_Blue = Inverted.f_Blue();
		}

		if (!(mp_Flags & EAnsiEncodingFlag_ColorSgrUsesSemiColon))
		{
			if (mp_Flags & EAnsiEncodingFlag_Color24Bit)
			{
				fp_StartSequence();
				CFStr24 Format = CFStr24::CFormat("48:2::{}:{}:{}") << _Red << _Green << _Blue;
				*mp_pAppender += Format;
				return 0;
			}

			auto Color = fg_FindColor256(_Red, _Green, _Blue);
			if (Color == _Previous)
				return Color;

			fp_StartSequence();
			CFStr24 Format = CFStr24::CFormat("48:5:{}") << Color;
			*mp_pAppender += Format;
			return Color;
		}
		else
		{
			if (mp_Flags & EAnsiEncodingFlag_Color24Bit)
			{
				fp_StartSequence();
				CFStr24 Format = CFStr24::CFormat("48;2;{};{};{}") << _Red << _Green << _Blue;
				*mp_pAppender += Format;
				return 0;
			}

			auto Color = fg_FindColor256(_Red, _Green, _Blue);
			if (Color == _Previous)
				return Color;

			fp_StartSequence();
			CFStr24 Format = CFStr24::CFormat("48;5;{}") << Color;
			*mp_pAppender += Format;
			return Color;
		}
	}

	uint32 CAnsiEncoding::CSgrSequence::f_UnderlineRGBDiff(uint32 _Previous, uint8 _Red, uint8 _Green, uint8 _Blue)
	{
		if (!(mp_Flags & EAnsiEncodingFlag_Color))
			return 0;

		if (mp_Flags & EAnsiEncodingFlag_ColorLightBackground)
		{
			NGraphics::CColorR8G8B8 Inverted = fg_InvertBrightness(_Red, _Green, _Blue);
			_Red = Inverted.f_Red();
			_Green = Inverted.f_Green();
			_Blue = Inverted.f_Blue();
		}

		if (!(mp_Flags & EAnsiEncodingFlag_ColorSgrUsesSemiColon))
		{
			if (mp_Flags & EAnsiEncodingFlag_Color24Bit)
			{
				fp_StartSequence();
				CFStr24 Format = CFStr24::CFormat("58:2::{}:{}:{}") << _Red << _Green << _Blue;
				*mp_pAppender += Format;
				return 0;
			}

			auto Color = fg_FindColor256(_Red, _Green, _Blue);
			if (Color == _Previous)
				return Color;

			fp_StartSequence();
			CFStr24 Format = CFStr24::CFormat("58:5:{}") << Color;
			*mp_pAppender += Format;
			return Color;
		}
		else
		{
			if (mp_Flags & EAnsiEncodingFlag_Color24Bit)
			{
				fp_StartSequence();
				CFStr24 Format = CFStr24::CFormat("58;2;{};{};{}") << _Red << _Green << _Blue;
				*mp_pAppender += Format;
				return 0;
			}

			auto Color = fg_FindColor256(_Red, _Green, _Blue);
			if (Color == _Previous)
				return Color;

			fp_StartSequence();
			CFStr24 Format = CFStr24::CFormat("58;5;{}") << Color;
			*mp_pAppender += Format;
			return Color;
		}
	}

	
	void CAnsiEncoding::CSgrSequence::f_Weight(EWeight _Weight)
	{
		if (!(mp_Flags & EAnsiEncodingFlag_Color))
			return;

		fp_StartSequence();

		switch (_Weight)
		{
		case EWeight::mc_Normal: *mp_pAppender += gc_Str<"22">.m_Str; break;
		case EWeight::mc_Bold: *mp_pAppender += gc_Str<"1">.m_Str; break;
		case EWeight::mc_Dim: *mp_pAppender += gc_Str<"2">.m_Str; break;
		case EWeight::mc_Shadowed: *mp_pAppender += gc_Str<"1:2">.m_Str; break;
		}
	}

	void CAnsiEncoding::CSgrSequence::f_Underline(EUnderline _Underline)
	{
		if (!(mp_Flags & EAnsiEncodingFlag_Color))
			return;

		fp_StartSequence();

		switch (_Underline)
		{
		case EUnderline::mc_None: *mp_pAppender += gc_Str<"24">.m_Str; break;
		case EUnderline::mc_Solid: *mp_pAppender += gc_Str<"4">.m_Str; break;
		case EUnderline::mc_Double: *mp_pAppender += gc_Str<"4:2">.m_Str; break;
		case EUnderline::mc_Wavy: *mp_pAppender += gc_Str<"4:3">.m_Str; break;
		case EUnderline::mc_Dotted: *mp_pAppender += gc_Str<"4:4">.m_Str; break;
		case EUnderline::mc_Dashed: *mp_pAppender += gc_Str<"4:5">.m_Str; break;
		}
	}

	void CAnsiEncoding::CSgrSequence::f_Italic()
	{
		if (!(mp_Flags & EAnsiEncodingFlag_Color))
			return;

		fp_StartSequence();

		*mp_pAppender += '3';
	}

	void CAnsiEncoding::CSgrSequence::f_NotItalic()
	{
		if (!(mp_Flags & EAnsiEncodingFlag_Color))
			return;

		fp_StartSequence();

		*mp_pAppender += gc_Str<"23">.m_Str;
	}

	void CAnsiEncoding::CSgrSequence::f_Strikeout()
	{
		if (!(mp_Flags & EAnsiEncodingFlag_Color))
			return;

		fp_StartSequence();

		*mp_pAppender += '9';
	}

	void CAnsiEncoding::CSgrSequence::f_NotStrikeout()
	{
		if (!(mp_Flags & EAnsiEncodingFlag_Color))
			return;

		fp_StartSequence();

		*mp_pAppender += gc_Str<"29">.m_Str;
	}
}
