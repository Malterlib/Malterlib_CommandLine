// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "Malterlib_CommandLine_AnsiEncodingParse.h"

#include <Mib/String/Appender>

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
			, TCFunction<bool (CStrPtr const &_String)> const &_fOnString
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

		// Text is emitted as views of the input string instead of accumulating into a temporary
		// buffer. Runs are committed when an escape byte is encountered, so the property-change
		// callbacks never have pending text to flush.
		const ch8 *pRunStart = _In;

		auto fSetColor = [&](CDecodedColor const &_Color, CDecodedColor const &_BGColor, CDecodedColor const &_UnderlineColor)
			{
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
				if (Weight != _Weight)
				{
					_fPropertyChange(CWeight{_Weight});
					Weight = _Weight;
				}
			}
		;
		auto fSetItalic = [&](bool _bItalic)
			{
				if (bItalic != _bItalic)
				{
					_fPropertyChange(CItalic{_bItalic});
					bItalic = _bItalic;
				}
			}
		;
		auto fSetUnderline = [&](CAnsiEncoding::EUnderline _Underline)
			{
				if (Underline != _Underline)
				{
					_fPropertyChange(CUnderline{_Underline});
					Underline = _Underline;
				}
			}
		;
		auto fSetStrikeout = [&](bool _bStrikeout)
			{
				if (bStrikeout != _bStrikeout)
				{
					_fPropertyChange(CStrikeout{_bStrikeout});
					bStrikeout = _bStrikeout;
				}
			}
		;
		auto fReset = [&]()
			{
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

		auto fCommitRun = [&](ch8 const *_pEnd)
			{
				if (_pEnd == pRunStart)
					return;

				bAborted = bAborted || !_fOnString(CStrPtr(pRunStart, _pEnd - pRunStart));
			}
		;

		while (*pParse && !bAborted)
		{
			ch8 Char = *pParse;
			if (Char == '\x1B')
			{
				fCommitRun(pParse);
				++pParse;
				if (*pParse == '[')
				{
					++pParse;
					auto pStart = pParse;
					while (*pStart && *pParse >= 0x30 && *pParse <= 0x3F)
						++pParse;
					CStrPtr Params(pStart, pParse - pStart);

					pStart = pParse;
					while (*pStart && *pParse >= 0x20 && *pParse <= 0x2F)
						++pParse;
					CStrPtr Intermediate(pStart, pParse - pStart);

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
								// The parameters are walked as views of the escape bytes: ';'
								// separates parameters (empty ones are skipped) and ':' separates
								// sub arguments (empty ones are kept), matching the previous
								// f_Split based parsing without temporary strings
								ch8 const *pParamPos = Params.f_GetStr();
								ch8 const *pParamsEnd = pParamPos + Params.f_GetLen();
								CStrPtr Param;

								auto fNextParam = [&]() -> bool
									{
										while (pParamPos < pParamsEnd)
										{
											ch8 const *pSeparator = pParamPos;
											while (pSeparator < pParamsEnd && *pSeparator != ';')
												++pSeparator;

											if (pSeparator != pParamPos)
											{
												Param = CStrPtr(pParamPos, pSeparator - pParamPos);
												pParamPos = pSeparator + 1;

												return true;
											}

											pParamPos = pSeparator + 1;
										}

										return false;
									}
								;

								constexpr umint c_MaxArguments = 16;
								CStrPtr Arguments[c_MaxArguments];
								umint nArguments = 0;

								auto fSplitArguments = [&]()
									{
										nArguments = 0;

										ch8 const *pPos = Param.f_GetStr();
										ch8 const *pEnd = pPos + Param.f_GetLen();
										for (;;)
										{
											ch8 const *pSeparator = pPos;
											while (pSeparator < pEnd && *pSeparator != ':')
												++pSeparator;

											if (nArguments < c_MaxArguments)
												Arguments[nArguments] = CStrPtr(pPos, pSeparator - pPos);
											++nArguments;

											if (pSeparator == pEnd)
												break;

											pPos = pSeparator + 1;
										}
									}
								;

								bool bHaveParam = fNextParam();
								while (bHaveParam)
								{
									uint32 ParamNumber = Param.f_ToInt(uint32(0), ":");

									fSplitArguments();
									if (nArguments == 1 && (ParamNumber == 38 || ParamNumber == 48 || ParamNumber == 58))
									{
										// Special case for broken encoding for true color
										bHaveParam = fNextParam();
										if (bHaveParam && Param == "5")
										{
											bHaveParam = fNextParam();
											if (bHaveParam)
											{
												if (ParamNumber == 38)
												{
													FgColor.f_SetAnsi256(Param.f_ToInt(uint8(0)));
													LastForeground = -1;
												}
												else if (ParamNumber == 48)
													BgColor.f_SetAnsi256(Param.f_ToInt(uint8(0)));
												else
													UnderlineColor.f_SetAnsi256(Param.f_ToInt(uint8(0)));
												bHaveParam = fNextParam();
											}
										}
										else if (bHaveParam && Param == "2")
										{
											bHaveParam = fNextParam();
											if (bHaveParam)
											{
												uint8 Red = Param.f_ToInt(uint8(0));
												bHaveParam = fNextParam();
												if (bHaveParam)
												{
													uint8 Green = Param.f_ToInt(uint8(0));
													bHaveParam = fNextParam();
													if (bHaveParam)
													{
														uint8 Blue = Param.f_ToInt(uint8(0));
														bHaveParam = fNextParam();
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
										if (nArguments > 1 && Arguments[1] == "2")
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
										if (nArguments > 1)
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
										umint iArgument = 1;

										if (iArgument < nArguments && Arguments[iArgument] == "5")
										{
											++iArgument;
											if (iArgument < nArguments)
											{
												if (ParamNumber == 38)
												{
													FgColor.f_SetAnsi256(Arguments[iArgument].f_ToInt(uint8(0)));
													LastForeground = -1;
												}
												else if (ParamNumber == 48)
													BgColor.f_SetAnsi256(Arguments[iArgument].f_ToInt(uint8(0)));
												else
													UnderlineColor.f_SetAnsi256(Arguments[iArgument].f_ToInt(uint8(0)));
												++iArgument;
											}
										}
										else if (iArgument < nArguments && Arguments[iArgument] == "2")
										{
											++iArgument;
											if (nArguments - iArgument >= 3)
											{
												// A length of exactly 3 means the colour space id slot was omitted
												if (nArguments - iArgument >= 4)
													++iArgument;
												auto Red = Arguments[iArgument].f_ToInt(uint8(0));
												auto Green = Arguments[++iArgument].f_ToInt(uint8(0));
												auto Blue = Arguments[++iArgument].f_ToInt(uint8(0));
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
										else if (iArgument < nArguments && Arguments[iArgument] == "3")
										{
											++iArgument;
											if (nArguments - iArgument >= 5)
											{
												fp64 Scale = Arguments[++iArgument].f_ToInt(uint8(0));
												auto CompC = fp64(Arguments[++iArgument].f_ToInt(uint8(0))) / Scale;
												auto CompM = fp64(Arguments[++iArgument].f_ToInt(uint8(0))) / Scale;
												auto CompY = fp64(Arguments[++iArgument].f_ToInt(uint8(0))) / Scale;
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
										else if (iArgument < nArguments && Arguments[iArgument] == "4")
										{
											++iArgument;
											if (nArguments - iArgument >= 6)
											{
												fp64 Scale = Arguments[++iArgument].f_ToInt(uint8(0));
												auto CompC = fp64(Arguments[++iArgument].f_ToInt(uint8(0))) / Scale;
												auto CompM = fp64(Arguments[++iArgument].f_ToInt(uint8(0))) / Scale;
												auto CompY = fp64(Arguments[++iArgument].f_ToInt(uint8(0))) / Scale;
												auto CompK = fp64(Arguments[++iArgument].f_ToInt(uint8(0))) / Scale;
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

									bHaveParam = fNextParam();
								}

								fSetColor(FgColor, BgColor, UnderlineColor);
							}
							break;
						}
					}
				}
				pRunStart = pParse;
				continue;
			}

			++pParse;
		}

		fCommitRun(pParse);
	}

	CStr CAnsiEncodingParse::fs_StripEncoding(CStr const &_In)
	{
		CStr Ret;
		{
			CStr::CAppender Appender(Ret);

			fs_Parse
				(
					_In
					, [&](CStrPtr const &_String) -> bool
					{
						Appender += _String;
						return true;
					}
					, [&](CPropertyChange const &_Change)
					{
					}
				)
			;
		}
		return Ret;
	}

	umint CAnsiEncodingParse::fs_RenderedStrLen(CStr const &_String)
	{
		umint Len = 0;
		fs_Parse
			(
				_String
				, [&](CStrPtr const &_String) -> bool
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
