// Copyright © 2015 Hansoft AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#include "Malterlib_CommandLine_AnsiEncoding.h"
#include "Malterlib_CommandLine_AnsiEncodingParse.h"

#include <Mib/Container/Regions>
#include <Mib/Storage/Optional>

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
		NStr::CStr g_Default = DMibCommandLineAnsiColor_Reset;

		NStr::CStr g_StatusNormal = DMibCommandLineAnsiColor_Reset DMibCommandLineAnsiColor_256(118);
		NStr::CStr g_StatusWarning = DMibCommandLineAnsiColor_Reset DMibCommandLineAnsiColor_256(207);
		NStr::CStr g_StatusError = DMibCommandLineAnsiColor_Reset DMibCommandLineAnsiColor_Bold DMibCommandLineAnsiColor_256(198);
		NStr::CStr g_Bold = DMibCommandLineAnsiColor_Bold;

		NStr::CStr g_Prompt = DMibCommandLineAnsiColor_Reset DMibCommandLineAnsiColor_256(221);

		uint8 fg_FindColor256(uint8 _Red, uint8 _Green, uint8 _Blue)
		{
			uint8 BestColor = 0;
			uint32 SmallestError = TCLimitsInt<uint32>::mc_Max;
			for (mint i = 16; i < 256; ++i)
			{
				auto &Color = g_CommandLine_AnsiEncodingColor256Array[i];

				uint32 ErrorRed = fg_Abs(int32(_Red) - int32(Color.m_Red));
				uint32 ErrorGreen = fg_Abs(int32(_Green) - int32(Color.m_Green));
				uint32 ErrorBlue = fg_Abs(int32(_Blue) - int32(Color.m_Blue));
				uint32 Error = ErrorRed * ErrorRed + ErrorGreen * ErrorGreen + ErrorBlue * ErrorBlue;
				if (Error < SmallestError)
				{
					SmallestError = Error;
					BestColor = i;
				}
			}
			return BestColor;

		}

		uint32 fg_InvertBrightness(uint8 _Red, uint8 _Green, uint8 _Blue)
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

			return Rf.f_ToIntRound() | Gf.f_ToIntRound() << 8 | Bf.f_ToIntRound() << 16;
		}
	}

	CAnsiEncoding::CAnsiEncoding(EAnsiEncodingFlag _Flags)
		: mp_Flags(_Flags)
	{
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

		return "\x1B[38;5;{}m"_f << _Color;
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

		return "\x1B[48;5;{}m"_f << _Color;
	}

	NStr::CStr CAnsiEncoding::f_ForegroundRGB(uint8 _Red, uint8 _Green, uint8 _Blue) const
	{
		if (!(mp_Flags & EAnsiEncodingFlag_Color))
			return {};

		if (mp_Flags & EAnsiEncodingFlag_ColorLightBackground)
		{
			uint32 Inverted = fg_InvertBrightness(_Red, _Green, _Blue);
			_Red = Inverted & 0xff;
			_Green = (Inverted >> 8) & 0xff;
			_Blue = (Inverted >> 16) & 0xff;
		}

		if (!(mp_Flags & EAnsiEncodingFlag_Color24Bit))
			return "\x1B[38;5;{}m"_f << fg_FindColor256(_Red, _Green, _Blue);

		return "\x1B[38;2;{};{};{}m"_f << _Red << _Green << _Blue;
	}

	NStr::CStr CAnsiEncoding::f_BackgroundRGB(uint8 _Red, uint8 _Green, uint8 _Blue) const
	{
		if (!(mp_Flags & EAnsiEncodingFlag_Color))
			return {};

		if (mp_Flags & EAnsiEncodingFlag_ColorLightBackground)
		{
			uint32 Inverted = fg_InvertBrightness(_Red, _Green, _Blue);
			_Red = Inverted & 0xff;
			_Green = (Inverted >> 8) & 0xff;
			_Blue = (Inverted >> 16) & 0xff;
		}

		if (!(mp_Flags & EAnsiEncodingFlag_Color24Bit))
			return "\x1B[48;5;{}m"_f << fg_FindColor256(_Red, _Green, _Blue);

		return "\x1B[48;2;{};{};{}m"_f << _Red << _Green << _Blue;
	}

	NStr::CStr CAnsiEncoding::f_Default() const
	{
		if (!(mp_Flags & EAnsiEncodingFlag_Color))
			return {};

		return g_Default;
	}

	NStr::CStr CAnsiEncoding::f_StatusNormal() const
	{
		if (!(mp_Flags & EAnsiEncodingFlag_Color))
			return {};
		if (mp_Flags & EAnsiEncodingFlag_ColorLightBackground)
			return DMibCommandLineAnsiColor_Reset + f_Foreground256(118);
		return g_StatusNormal;
	}

	NStr::CStr CAnsiEncoding::f_StatusWarning() const
	{
		if (!(mp_Flags & EAnsiEncodingFlag_Color))
			return {};
		if (mp_Flags & EAnsiEncodingFlag_ColorLightBackground)
			return DMibCommandLineAnsiColor_Reset + f_Foreground256(207);
		return g_StatusWarning;
	}

	NStr::CStr CAnsiEncoding::f_StatusError() const
	{
		if (!(mp_Flags & EAnsiEncodingFlag_Color))
			return {};
		if (mp_Flags & EAnsiEncodingFlag_ColorLightBackground)
			return DMibCommandLineAnsiColor_Reset DMibCommandLineAnsiColor_Bold + f_Foreground256(198);
		return g_StatusError;
	}

	NStr::CStr CAnsiEncoding::f_Bold() const
	{
		if (!(mp_Flags & EAnsiEncodingFlag_Color))
			return {};
		return g_Bold;
	}

	NStr::CStr CAnsiEncoding::f_Prompt() const
	{
		if (!(mp_Flags & EAnsiEncodingFlag_Color))
			return {};

		if (mp_Flags & EAnsiEncodingFlag_ColorLightBackground)
			return DMibCommandLineAnsiColor_Reset + f_Foreground256(221);

		return g_Prompt;
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
				, [&](CStr const &_String)
			 	{
					Ret += _String;
				}
				, [&](CAnsiEncodingParse::CPropertyChange const &_Change)
			 	{
					if (!f_Color())
						return;

					if (_Change.f_IsOfType<CAnsiEncodingParse::CReset>())
						Ret += f_Default();
					else if (_Change.f_IsOfType<CAnsiEncodingParse::CBold>() && _Change.f_GetAsType<CAnsiEncodingParse::CBold>().m_bEnabled)
						Ret += f_Bold();
					else if (_Change.f_IsOfType<CAnsiEncodingParse::CForegroundColor>())
					{
						auto &Color = _Change.f_GetAsType<CAnsiEncodingParse::CForegroundColor>();
						Ret += f_ForegroundRGB(Color.m_Red, Color.m_Green, Color.m_Blue);
					}
					else if (_Change.f_IsOfType<CAnsiEncodingParse::CBackgroundColor>())
					{
						auto &Color = _Change.f_GetAsType<CAnsiEncodingParse::CBackgroundColor>();
						Ret += f_BackgroundRGB(Color.m_Red, Color.m_Green, Color.m_Blue);
					}
				}
			)
		;
		return Ret;
	}

	TCVector<CStr> CAnsiEncoding::f_LineBreak(CStr const &_String, mint _Length) const
	{
		if (_String.f_Trim().f_IsEmpty())
			return {""};

		struct CProperties
		{
			TCOptional<CAnsiEncodingParse::CBackgroundColor> m_BackgroundColor;
			TCOptional<CAnsiEncodingParse::CForegroundColor> m_ForegroundColor;
			TCOptional<CAnsiEncodingParse::CBold> m_Bold;

			auto f_Tuple() const
			{
				return fg_TupleReferences(m_BackgroundColor, m_ForegroundColor, m_Bold);
			}

			bool operator == (CProperties const &_Right) const
			{
				return f_Tuple() == _Right.f_Tuple();
			}
		};

		TCRegions<mint, CProperties> PropertyRegions;

		CProperties CurrentProperties;

		CUStr String;

		CAnsiEncodingParse::fs_Parse
			(
				_String
				, [&](CUStr _String)
			 	{
					auto Start = String.f_GetLen();
					String += _String;
					PropertyRegions.f_MakeRegion
						(
						 	Start
						 	, String.f_GetLen()
						 	, [&](CProperties &o_Properties)
						 	{
								o_Properties = CurrentProperties;
							}
						)
					;
				}
				, [&](CAnsiEncodingParse::CPropertyChange const &_Change)
			 	{
					if (_Change.f_IsOfType<CAnsiEncodingParse::CReset>())
					{
						CurrentProperties.m_BackgroundColor.f_Clear();
						CurrentProperties.m_ForegroundColor.f_Clear();
						CurrentProperties.m_Bold.f_Clear();
					}
					else if (_Change.f_IsOfType<CAnsiEncodingParse::CBold>() && _Change.f_GetAsType<CAnsiEncodingParse::CBold>().m_bEnabled)
						CurrentProperties.m_Bold = {true};
					else if (_Change.f_IsOfType<CAnsiEncodingParse::CBackgroundColor>())
					{
						auto &Color = _Change.f_GetAsType<CAnsiEncodingParse::CBackgroundColor>();
						if (Color.m_bEnabled)
							CurrentProperties.m_BackgroundColor= Color;
						else
							CurrentProperties.m_BackgroundColor.f_Clear();
					}
					else if (_Change.f_IsOfType<CAnsiEncodingParse::CForegroundColor>())
					{
						auto &Color = _Change.f_GetAsType<CAnsiEncodingParse::CForegroundColor>();
						if (Color.m_bEnabled)
							CurrentProperties.m_ForegroundColor = Color;
						else
							CurrentProperties.m_ForegroundColor.f_Clear();
					}
				}
			)
		;

		ch32 const *pParse = String;
		ch32 const *pParseStart = pParse;
		ch32 const *pLastWord = nullptr;

		mint Len = 0;
		mint MaxLen = _Length;

		TCVector<CStr> Output;

		auto fOutputLine = [&](ch32 const *_pStart, mint _Len)
			{
				mint Position = _pStart - String.f_GetStr();
				mint EndPos = Position + _Len;
				auto iRegion = PropertyRegions.f_GetIteratorLower(Position);
				if (iRegion && iRegion->f_Start() < EndPos)
				{
					CStr ToOutput;
					ch32 const *pParseEnd = _pStart + _Len;
					bool bSetProperties = false;
					for (ch32 const *pParse = _pStart; pParse < pParseEnd;)
					{
						auto &Properties = iRegion->f_Data();
						if (bSetProperties)
						{
							ToOutput += f_Default();
							bSetProperties = false;
						}

						if (Properties.m_Bold && Properties.m_Bold->m_bEnabled)
						{
							ToOutput += f_Bold();
							bSetProperties = true;
						}

						if (Properties.m_BackgroundColor && Properties.m_BackgroundColor->m_bEnabled)
						{
							ToOutput += f_BackgroundRGB(Properties.m_BackgroundColor->m_Red, Properties.m_BackgroundColor->m_Green, Properties.m_BackgroundColor->m_Blue);
							bSetProperties = true;
						}

						if (Properties.m_ForegroundColor && Properties.m_ForegroundColor->m_bEnabled)
						{
							ToOutput += f_ForegroundRGB(Properties.m_ForegroundColor->m_Red, Properties.m_ForegroundColor->m_Green, Properties.m_ForegroundColor->m_Blue);
							bSetProperties = true;
						}

						mint CurrentPosition = pParse - String.f_GetStr();
						mint Len = pParseEnd - pParse;
						mint EndPosition = CurrentPosition + Len;
						mint RegionEnd = iRegion->f_End();
						mint ToAdd;

						if (RegionEnd < EndPosition)
							ToAdd = fg_Min(RegionEnd - CurrentPosition, Len);
						else
							ToAdd = Len;

						ToOutput.f_AddStr(pParse, ToAdd);
						pParse += ToAdd;

						CurrentPosition += ToAdd;
						if (CurrentPosition == RegionEnd)
							++iRegion;
					}

					if (bSetProperties)
						ToOutput += f_Default();

					Output.f_Insert(ToOutput);
				}
				else
					Output.f_Insert(CUStr{_pStart, _Len});
			}
		;

		while (*pParse)
		{
			ch32 Char = *pParse;
			++pParse;
			++Len;
			while
			(
				(Char >= 0x0300 && Char <= 0x036F)
				|| (Char >= 0x1AB0 && Char <= 0x1AFF)
				|| (Char >= 0x1DC0 && Char <= 0x1DFF)
				|| (Char >= 0x20D0 && Char <= 0x20FF)
				|| (Char >= 0xFE20 && Char <= 0xFE2F)
			)
			{
				Char = *pParse;
				if (!Char)
					break;
				++pParse;
			}

			if (Len == MaxLen)
			{
				if (pLastWord)
				{
					fOutputLine(pParseStart, pLastWord - pParseStart);
					pParseStart = pLastWord;
					while (fg_CharIsWhiteSpace(*pParseStart))
						++pParseStart;
					pParse = pParseStart;
				}
				else
				{
					fOutputLine(pParseStart, pParse - pParseStart);
					pParseStart = pParse;
				}

				pLastWord = nullptr;
				Len = 0;
			}

			if (fg_CharIsWhiteSpace(Char))
				pLastWord = pParse;
		}

		if (pParseStart != pParse)
			fOutputLine(pParseStart, pParse - pParseStart);

		return Output;
	}
}
