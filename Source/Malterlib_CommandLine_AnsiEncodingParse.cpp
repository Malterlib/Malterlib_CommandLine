// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

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

		CAnsiEncoding::EWeight &Weight = pParseState->m_Weight;
		CAnsiEncoding::EUnderline &Underline = pParseState->m_Underline;
		bool &bItalic = pParseState->m_bItalic;
		bool &bStrikeout = pParseState->m_bStrikeout;
		bool &bAborted = pParseState->m_bAborted;
		int32 &LastForeground = pParseState->m_LastForeground;
		CDecodedColor &CurrentColor = pParseState->m_CurrentColor;
		CDecodedColor &CurrentColorBG = pParseState->m_CurrentColorBG;
		CDecodedColor &CurrentColorUnderline = pParseState->m_CurrentColorUnderline;

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

		auto fSetColor = [&](CDecodedColor const &_Color, CDecodedColor const &_BGColor, CDecodedColor const &_UnderlineColor)
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
				if (_UnderlineColor != CurrentColorUnderline)
				{
					_fPropertyChange(CUnderlineColor{_UnderlineColor});
					CurrentColorUnderline = _UnderlineColor;
				}
			}
		;
		auto fSetWeight = [&](CAnsiEncoding::EWeight _Weight)
			{
				fCommitBuffer();
				if (Weight != _Weight)
				{
					_fPropertyChange(CWeight{_Weight});
					Weight = _Weight;
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
		auto fSetUnderline = [&](CAnsiEncoding::EUnderline _Underline)
			{
				fCommitBuffer();
				if (Underline != _Underline)
				{
					_fPropertyChange(CUnderline{_Underline});
					Underline = _Underline;
				}
			}
		;
		auto fSetStrikeout = [&](bool _bStrikeout)
			{
				fCommitBuffer();
				if (bStrikeout != _bStrikeout)
				{
					_fPropertyChange(CStrikeout{_bStrikeout});
					bStrikeout = _bStrikeout;
				}
			}
		;
		auto fReset = [&]()
			{
				fCommitBuffer();
				fSetWeight(CAnsiEncoding::EWeight::mc_Normal);
				fSetItalic(false);
				fSetUnderline(CAnsiEncoding::EUnderline::mc_None);
				fSetStrikeout(false);
				_fPropertyChange(CReset{});

				// Clear the tracked colors so colors selected again after the reset are emitted
				CurrentColor = {};
				CurrentColorBG = {};
				CurrentColorUnderline = {};
			}
		;

		const ch8 *pParse = _In;
		while (*pParse && !bAborted)
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
							CDecodedColor UnderlineColor = CurrentColorUnderline;

							if (Params.f_IsEmpty() && Intermediate.f_IsEmpty())
							{
								fReset();
								fSetColor({}, {}, {});
							}
							else
							{
								auto ParamsVector = Params.f_Split<true>(";");

								for (auto iParam = ParamsVector.f_GetIterator(); iParam;)
								{
									uint32 ParamNumber = iParam->f_ToInt(uint32(0), ":");

									auto Arguments = iParam->f_Split(":");
									if (Arguments.f_GetLen() == 1 && (ParamNumber == 38 || ParamNumber == 48 || ParamNumber == 58))
									{
										// Special case for broken encoding for true color
										++iParam;
										if (iParam && *iParam == "5")
										{
											++iParam;
											if (iParam)
											{
												if (ParamNumber == 38)
												{
													FgColor.f_SetAnsi256(iParam->f_ToInt(uint8(0)));
													LastForeground = -1;
												}
												else if (ParamNumber == 48)
													BgColor.f_SetAnsi256(iParam->f_ToInt(uint8(0)));
												else
													UnderlineColor.f_SetAnsi256(iParam->f_ToInt(uint8(0)));
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
														{
															FgColor.f_Set(Red, Green, Blue);
															LastForeground = -1;
														}
														else if (ParamNumber == 48)
															BgColor.f_Set(Red, Green, Blue);
														else
															UnderlineColor.f_Set(Red, Green, Blue);
													}
												}
											}
										}
										continue;
									}

									if (ParamNumber == 1)
									{
										if (Arguments.f_GetLen() > 1 && Arguments[1] == "2")
											fSetWeight(CAnsiEncoding::EWeight::mc_Shadowed);
										else
											fSetWeight(CAnsiEncoding::EWeight::mc_Bold);
										if (LastForeground >= 30 && LastForeground <= 37)
											ParamNumber = LastForeground + 60;
									}
									else if (ParamNumber == 2)
										fSetWeight(CAnsiEncoding::EWeight::mc_Dim);
									else if (ParamNumber == 22)
									{
										fSetWeight(CAnsiEncoding::EWeight::mc_Normal);
										if (LastForeground >= 90 && LastForeground <= 97)
											ParamNumber = LastForeground - 60;
									}
									else if (ParamNumber == 3)
										fSetItalic(true);
									else if (ParamNumber == 23)
										fSetItalic(false);
									else if (ParamNumber == 4)
									{
										auto UnderlineStyle = CAnsiEncoding::EUnderline::mc_Solid;
										if (Arguments.f_GetLen() > 1)
										{
											switch (Arguments[1].f_ToInt(uint32(0)))
											{
											case 0: UnderlineStyle = CAnsiEncoding::EUnderline::mc_None; break;
											case 1: UnderlineStyle = CAnsiEncoding::EUnderline::mc_Solid; break;
											case 2: UnderlineStyle = CAnsiEncoding::EUnderline::mc_Double; break;
											case 3: UnderlineStyle = CAnsiEncoding::EUnderline::mc_Wavy; break;
											case 4: UnderlineStyle = CAnsiEncoding::EUnderline::mc_Dotted; break;
											case 5: UnderlineStyle = CAnsiEncoding::EUnderline::mc_Dashed; break;
											}
										}

										fSetUnderline(UnderlineStyle);
									}
									else if (ParamNumber == 24)
										fSetUnderline(CAnsiEncoding::EUnderline::mc_None);
									else if (ParamNumber == 9)
										fSetStrikeout(true);
									else if (ParamNumber == 29)
										fSetStrikeout(false);
									else if (ParamNumber >= 30 && ParamNumber <= 37)
									{
										LastForeground = ParamNumber;
										if (Weight == CAnsiEncoding::EWeight::mc_Bold)
											ParamNumber += 60;
									}
									else if (ParamNumber == 38 || ParamNumber == 48 || ParamNumber == 58)
									{
										auto iArgument = Arguments.f_GetIterator();
										++iArgument;

										if (iArgument && *iArgument == "5")
										{
											++iArgument;
											if (iArgument)
											{
												if (ParamNumber == 38)
												{
													FgColor.f_SetAnsi256(iArgument->f_ToInt(uint8(0)));
													LastForeground = -1;
												}
												else if (ParamNumber == 48)
													BgColor.f_SetAnsi256(iArgument->f_ToInt(uint8(0)));
												else
													UnderlineColor.f_SetAnsi256(iArgument->f_ToInt(uint8(0)));
												++iArgument;
											}
										}
										else if (iArgument && *iArgument == "2")
										{
											++iArgument;
											if (iArgument.f_GetLen() >= 3)
											{
												// A length of exactly 3 means the colour space id slot was omitted
												if (iArgument.f_GetLen() >= 4)
													++iArgument;
												auto Red = iArgument->f_ToInt(uint8(0));
												auto Green = (++iArgument)->f_ToInt(uint8(0));
												auto Blue = (++iArgument)->f_ToInt(uint8(0));
												if (ParamNumber == 38)
												{
													FgColor.f_Set(Red, Green, Blue);
													LastForeground = -1;
												}
												else if (ParamNumber == 48)
													BgColor.f_Set(Red, Green, Blue);
												else
													UnderlineColor.f_Set(Red, Green, Blue);
											}
										}
										else if (iArgument && *iArgument == "3")
										{
											++iArgument;
											if (iArgument.f_GetLen() >= 5)
											{
												fp64 Scale = (++iArgument)->f_ToInt(uint8(0));
												auto CompC = fp64((++iArgument)->f_ToInt(uint8(0))) / Scale;
												auto CompM = fp64((++iArgument)->f_ToInt(uint8(0))) / Scale;
												auto CompY = fp64((++iArgument)->f_ToInt(uint8(0))) / Scale;
												uint8 Red = ((fp64(1.0) - CompC) * 255.0).f_ToInt();
												uint8 Green = ((fp64(1.0) - CompM) * 255.0).f_ToInt();
												uint8 Blue = ((fp64(1.0) - CompY) * 255.0).f_ToInt();
												if (ParamNumber == 38)
												{
													FgColor.f_Set(Red, Green, Blue);
													LastForeground = -1;
												}
												else if (ParamNumber == 48)
													BgColor.f_Set(Red, Green, Blue);
												else
													UnderlineColor.f_Set(Red, Green, Blue);
											}
										}
										else if (iArgument && *iArgument == "4")
										{
											++iArgument;
											if (iArgument.f_GetLen() >= 6)
											{
												fp64 Scale = (++iArgument)->f_ToInt(uint8(0));
												auto CompC = fp64((++iArgument)->f_ToInt(uint8(0))) / Scale;
												auto CompM = fp64((++iArgument)->f_ToInt(uint8(0))) / Scale;
												auto CompY = fp64((++iArgument)->f_ToInt(uint8(0))) / Scale;
												auto CompK = fp64((++iArgument)->f_ToInt(uint8(0))) / Scale;
												uint8 Red = ((fp64(1.0) - fg_Min(CompC + CompK, fp64(1.0))) * 255.0).f_ToInt();
												uint8 Green = ((fp64(1.0) - fg_Min(CompM + CompK, fp64(1.0))) * 255.0).f_ToInt();
												uint8 Blue = ((fp64(1.0) - fg_Min(CompY + CompK, fp64(1.0))) * 255.0).f_ToInt();
												if (ParamNumber == 38)
												{
													FgColor.f_Set(Red, Green, Blue);
													LastForeground = -1;
												}
												else if (ParamNumber == 48)
													BgColor.f_Set(Red, Green, Blue);
												else
													UnderlineColor.f_Set(Red, Green, Blue);
											}
										}
									}
									else
										LastForeground = -1;

									switch (ParamNumber)
									{
									case 0: FgColor = {}; BgColor = {}; UnderlineColor = {}; fReset(); break;
									case 39: FgColor = {}; break;
									case 49: BgColor = {}; break;
									case 59: UnderlineColor = {}; break;

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

								fSetColor(FgColor, BgColor, UnderlineColor);
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
