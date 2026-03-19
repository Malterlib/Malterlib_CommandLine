// Copyright © 2015 Hansoft AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#include "Malterlib_CommandLine_AnsiEncodingParse.h"

namespace NMib::NCommandLine
{
	using namespace NStr;
	using namespace NFunction;

	extern CAnsiEncodingParse::CDecodedColor g_CommandLine_AnsiEncodingColor256Array[256];

	void CAnsiEncodingParse::CDecodedColor::f_Set(uint8 _Red, uint8 _Green, uint8 _Blue)
	{
		m_Red = _Red;
		m_Green = _Green;
		m_Blue = _Blue;
		m_bEnabled = true;
	}

	void CAnsiEncodingParse::CDecodedColor::f_SetAnsi16(uint8 _Value)
	{
		*this = g_CommandLine_AnsiEncodingColor256Array[_Value];
	}

	void CAnsiEncodingParse::CDecodedColor::f_SetAnsi256(uint8 _Value)
	{
		*this = g_CommandLine_AnsiEncodingColor256Array[_Value];
	}

	void CAnsiEncodingParse::fs_Parse
		(
			CStr const &_In
			, TCFunction<bool (CStr const &_String)> const &_fOnString
			, TCFunction<void (CPropertyChange const &_Property)> const &_fPropertyChange
			, CParseState *_pParseState
		)
	{
		CParseState ParseState;
		CParseState *pParseState = _pParseState ? _pParseState : &ParseState;

		bool &bBold = pParseState->m_bBold;
		bool &bItalic = pParseState->m_bItalic;
		bool &bAborted = pParseState->m_bAborted;
		int32 &LastForeground = pParseState->m_LastForeground;
		CDecodedColor &CurrentColor = pParseState->m_CurrentColor;
		CDecodedColor &CurrentColorBG = pParseState->m_CurrentColorBG;

		CStr StringBuffer;

		auto fCommitBuffer = [&]
			{
				if (!StringBuffer.f_IsEmpty())
				{
					bAborted = bAborted || !_fOnString(StringBuffer);
					StringBuffer.f_Clear();
				}
			}
		;

		auto fSetColor = [&](CDecodedColor const &_Color, CDecodedColor const &_BGColor)
			{
				fCommitBuffer();
				if (_Color != CurrentColor)
				{
					_fPropertyChange(CForegroundColor{_Color});
					CurrentColor = _Color;
				}
				if (_BGColor != CurrentColorBG)
				{
					_fPropertyChange(CBackgroundColor{_BGColor});
					CurrentColorBG = _BGColor;
				}
			}
		;
		auto fSetBold = [&](bool _bBold)
			{
				fCommitBuffer();
				if (bBold != _bBold)
				{
					_fPropertyChange(CBold{_bBold});
					bBold = _bBold;
				}
			}
		;
		auto fSetItalic = [&](bool _bItalic)
			{
				fCommitBuffer();
				if (bItalic != _bItalic)
				{
					_fPropertyChange(CItalic{_bItalic});
					bItalic = _bItalic;
				}
			}
		;
		auto fReset = [&]()
			{
				fCommitBuffer();
				fSetBold(false);
				fSetItalic(false);
				_fPropertyChange(CReset{});
			}
		;

		const ch8 *pParse = _In;
		while (*pParse &&!bAborted)
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

							if (Params.f_IsEmpty() && Intermediate.f_IsEmpty())
							{
								fReset();
								fSetColor({}, {});
							}
							else
							{
								auto ParamsVector = Params.f_Split<true>(";");

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
										fSetBold(true);
										if (LastForeground >= 30 && LastForeground <= 37)
											ParamNumber = LastForeground + 60;
									}
									else if (ParamNumber == 22)
									{
										fSetBold(false);
										if (LastForeground >= 90 && LastForeground <= 97)
											ParamNumber = LastForeground - 60;
									}
									else if (ParamNumber == 3)
										fSetItalic(true);
									else if (ParamNumber == 23)
										fSetItalic(false);
									else if (ParamNumber >= 30 && ParamNumber <= 37)
									{
										LastForeground = ParamNumber;
										if (bBold)
											ParamNumber += 60;
									}
									else
										LastForeground = -1;

									switch (ParamNumber)
									{
									case 0: FgColor = {}; BgColor = {}; fReset(); break;
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
							}
							break;
						}
					}
				}
				continue;
			}
			else
				StringBuffer.f_AddChar(Char);

			++pParse;
		}

		fCommitBuffer();
	}

	CStr CAnsiEncodingParse::fs_StripEncoding(CStr const &_In)
	{
		CStr Ret;

		fs_Parse
			(
				_In
				, [&](CStr const &_String) -> bool
				{
					Ret += _String;
					return true;
				}
				, [&](CPropertyChange const &_Change)
				{
				}
			)
		;
		return Ret;
	}

	umint CAnsiEncodingParse::fs_RenderedStrLen(CStr const &_String)
	{
		umint Len = 0;
		fs_Parse
			(
				_String
				, [&](CStr const &_String) -> bool
				{
					for (auto iParse = _String.f_GetUnicodeIterator(); iParse; ++iParse)
					{
						ch32 Char = *iParse;
						if (!fg_CharIsCombining(Char))
						{
							++Len;
						}
					}

					return true;
				}
				, [&](CPropertyChange const &_Change)
				{
				}
			)
		;
		return Len;
	}
}
