// Copyright © 2015 Hansoft AB 
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#include "Malterlib_CommandLine_AnsiEncoding.h"

namespace NMib::NCommandLine
{
	using namespace NStr;
	using namespace NFunction;

	void CAnsiEncoding::CDecodedColor::f_Set(uint8 _Red, uint8 _Green, uint8 _Blue)
	{
		m_Red = _Red;
		m_Green = _Green;
		m_Blue = _Blue;
		m_bEnabled = true;
	}

	void CAnsiEncoding::CDecodedColor::f_SetAnsi16(uint8 _Value)
	{
		switch (_Value)
		{
		case 0: f_Set(0,0,0); break;
		case 1: f_Set(194,54,33); break;
		case 2: f_Set(37,188,36); break;
		case 3: f_Set(173,173,39); break;
		case 4: f_Set(73,46,225); break;
		case 5: f_Set(211,56,211); break;
		case 6: f_Set(51,187,200); break;
		case 7: f_Set(203,204,205); break;
		case 8: f_Set(129,131,131); break;
		case 9: f_Set(252,57,31); break;
		case 10: f_Set(49,231,34); break;
		case 11: f_Set(234,236,35); break;
		case 12: f_Set(88,51,255); break;
		case 13: f_Set(249,53,248); break;
		case 14: f_Set(20,240,240); break;
		case 15: f_Set(233,235,235); break;
		}
	}

	void CAnsiEncoding::CDecodedColor::f_SetAnsi256(uint8 _Value)
	{
		if (_Value < 16)
			return f_SetAnsi16(_Value);
		if (_Value >= 232)
		{
			uint8 Color = (_Value - 232) * 10 + 8;
			return f_Set(Color, Color, Color);
		}

		_Value -= 16;

		//6 × 6 × 6 cube (216 colors): 16 + 36 × r + 6 × g + b (0 ≤ r, g, b ≤ 5)

		uint8 Red = ((_Value / 36) * 255) / 5;
		uint8 Reminder = _Value % 36;
		uint8 Green = ((Reminder / 6) * 255) / 5;
		uint8 Blue = ((Reminder % 6) * 255) / 5;

		return f_Set(Red, Green, Blue);
	}

	void CAnsiEncoding::fs_Parse
		(
		 	CStr const &_In
		 	, TCFunction<void (CStr const &_String)> const &_fOnString
		 	, TCFunction<void (CDecodedColor const &_Color, CDecodedColor const &_BGColor)> const &_fOnColor
		)
	{
		bool bBold = false;
		int32 LastForeground = -1;
		CDecodedColor CurrentColor;
		CDecodedColor CurrentColorBG;

		CStr StringBuffer;

		auto fSetColor = [&](CDecodedColor const &_Color, CDecodedColor const &_BGColor)
			{
				if (!StringBuffer.f_IsEmpty())
				{
					_fOnString(StringBuffer);
					StringBuffer.f_Clear();
				}
				if (CurrentColor.m_bEnabled || CurrentColor.m_bEnabled || _Color.m_bEnabled || _BGColor.m_bEnabled)
					_fOnColor(_Color, _BGColor);
				CurrentColor = _Color;
				CurrentColorBG = _BGColor;
			}
		;

		const ch8 *pParse = _In;
		bint IsBeginning = true;
		while (*pParse)
		{
			ch8 Char = *pParse;
			if (Char == '\x1B')
			{
				++pParse;
				if (*pParse == '[')
				{
					++pParse;
					auto pStart = pParse;
					while (*pStart && *pParse >= 0x30 && *pParse <= 0x3F)
						++pParse;
					CStr Params(pStart, pParse - pStart);

					pStart = pParse;
					while (*pStart && *pParse >= 0x20 && *pParse <= 0x2F)
						++pParse;
					CStr Intermediate(pStart, pParse - pStart);

					char Final = *pParse;
					++pParse;

					switch (Final)
					{
					case 'm':
						{
							// SGR

							CDecodedColor FgColor = CurrentColor;
							CDecodedColor BgColor = CurrentColorBG;

							auto ParamsVector = Params.f_Split(";");

							for (auto iParam = ParamsVector.f_GetIterator(); iParam;)
							{
								uint32 ParamNumber = iParam->f_ToInt(uint32(0));
								if (ParamNumber == 38 || ParamNumber == 48)
								{
									++iParam;
									if (iParam && *iParam == "5")
									{
										++iParam;
										if (iParam)
										{
											if (ParamNumber == 38)
												FgColor.f_SetAnsi256(iParam->f_ToInt(uint8(0)));
											else
												BgColor.f_SetAnsi256(iParam->f_ToInt(uint8(0)));
											++iParam;
										}
									}
									else if (iParam && *iParam == "2")
									{
										++iParam;
										if (iParam)
										{
											uint8 Red = iParam->f_ToInt(uint8(0));
											++iParam;
											if (iParam)
											{
												uint8 Green = iParam->f_ToInt(uint8(0));
												++iParam;
												if (iParam)
												{
													uint8 Blue = iParam->f_ToInt(uint8(0));
													++iParam;
													if (ParamNumber == 38)
														FgColor.f_Set(Red, Green, Blue);
													else
														BgColor.f_Set(Red, Green, Blue);
												}
											}
										}
									}
									continue;
								}

								if (ParamNumber == 1)
								{
									bBold = true;
									if (LastForeground >= 30 && ParamNumber <= 37)
										ParamNumber = LastForeground + 10;
								}
								else if (ParamNumber >= 30 && ParamNumber <= 37)
								{
									LastForeground = ParamNumber;
									if (bBold)
										ParamNumber += 10;
								}
								else
									LastForeground = -1;

								switch (ParamNumber)
								{
								case 0: FgColor = {}; BgColor = {}; bBold = false; break;
								case 1: FgColor = {}; BgColor = {}; bBold = false; break;
								case 39: FgColor = {}; break;
								case 49: BgColor = {}; break;

								case 30: FgColor.f_SetAnsi16(0); break;
								case 40: BgColor.f_SetAnsi16(0); break;
								case 31: FgColor.f_SetAnsi16(1); break;
								case 41: BgColor.f_SetAnsi16(1); break;
								case 32: FgColor.f_SetAnsi16(2); break;
								case 42: BgColor.f_SetAnsi16(2); break;
								case 33: FgColor.f_SetAnsi16(3); break;
								case 43: BgColor.f_SetAnsi16(3); break;
								case 34: FgColor.f_SetAnsi16(4); break;
								case 44: BgColor.f_SetAnsi16(4); break;
								case 35: FgColor.f_SetAnsi16(5); break;
								case 45: BgColor.f_SetAnsi16(5); break;
								case 36: FgColor.f_SetAnsi16(6); break;
								case 46: BgColor.f_SetAnsi16(6); break;
								case 37: FgColor.f_SetAnsi16(7); break;
								case 47: BgColor.f_SetAnsi16(7); break;

								case 90: FgColor.f_SetAnsi16(8); break;
								case 100: BgColor.f_SetAnsi16(8); break;
								case 91: FgColor.f_SetAnsi16(9); break;
								case 101: BgColor.f_SetAnsi16(9); break;
								case 92: FgColor.f_SetAnsi16(10); break;
								case 102: BgColor.f_SetAnsi16(10); break;
								case 93: FgColor.f_SetAnsi16(11); break;
								case 103: BgColor.f_SetAnsi16(11); break;
								case 94: FgColor.f_SetAnsi16(12); break;
								case 104: BgColor.f_SetAnsi16(12); break;
								case 95: FgColor.f_SetAnsi16(13); break;
								case 105: BgColor.f_SetAnsi16(13); break;
								case 96: FgColor.f_SetAnsi16(14); break;
								case 106: BgColor.f_SetAnsi16(14); break;
								case 97: FgColor.f_SetAnsi16(15); break;
								case 107: BgColor.f_SetAnsi16(15); break;
								}
								++iParam;
							}

							fSetColor(FgColor, BgColor);
							break;
						}
					}
				}
				continue;
			}
			else
				StringBuffer.f_AddChar(Char);

			IsBeginning = false;

			++pParse;
		}

		fSetColor({}, {});
	}

	CStr CAnsiEncoding::fs_StripEncoding(CStr const &_In)
	{
		CStr Ret;

		fs_Parse
			(
				_In
				, [&](CStr const &_String)
			 	{
					Ret += _String;
				}
				, [&](CDecodedColor const &_Color, CDecodedColor const &_BGColor)
			 	{
				}
			)
		;
		return Ret;
	}
}
