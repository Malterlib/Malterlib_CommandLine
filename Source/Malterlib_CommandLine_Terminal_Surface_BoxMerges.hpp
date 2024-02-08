// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

namespace NMib::NCommandLine
{
#define DCalculateBoxMerges 0
#define DOutputBoxMerges 0

	using namespace NStr;
	namespace
	{
#if DCalculateBoxMerges
		struct CBoxMergeSource
		{
			CStr m_Left;
			CStr m_Right;
			CStr m_Same;
			ch8 const *m_pFile = nullptr;
			ch32 m_Combined = 0;
			int32 m_Line = 0;
			ETerminalEndpointType m_EndpointTypesLeft[14] = {ETerminalEndpointType::mc_Unspecified};
			ETerminalEndpointType m_EndpointTypesRight[14] = {ETerminalEndpointType::mc_Unspecified};
		};

		#define DInsertFileLine , .m_pFile = DMibPFile \
								, .m_Line = DMibPLine

		#define DLightRoundedOnward ETerminalEndpointType::mc_LightRounded, \
			ETerminalEndpointType::mc_LightDotted3Rounded, ETerminalEndpointType::mc_LightDotted2Rounded, \
			ETerminalEndpointType::mc_LightDotted1Rounded

		#define DLightRoundedOnwardNoOpaque  ETerminalEndpointType::mc_LightDotted3Rounded, ETerminalEndpointType::mc_LightDotted2Rounded, \
			ETerminalEndpointType::mc_LightDotted1Rounded

		#define DLightAllDotted3Onward ETerminalEndpointType::mc_LightDotted3, ETerminalEndpointType::mc_LightDotted2, \
			ETerminalEndpointType::mc_LightDotted1, ETerminalEndpointType::mc_LightDotted3Rounded, \
			ETerminalEndpointType::mc_LightDotted2Rounded, ETerminalEndpointType::mc_LightDotted1Rounded

		#define DLightAllDotted2Onward ETerminalEndpointType::mc_LightDotted2, \
			ETerminalEndpointType::mc_LightDotted1, \
			ETerminalEndpointType::mc_LightDotted2Rounded, ETerminalEndpointType::mc_LightDotted1Rounded

		#define DLightAllDotted1Onward ETerminalEndpointType::mc_LightDotted1, ETerminalEndpointType::mc_LightDotted1Rounded

		#define DLightOnward ETerminalEndpointType::mc_Light, \
			ETerminalEndpointType::mc_LightDotted3, ETerminalEndpointType::mc_LightDotted2, \
			ETerminalEndpointType::mc_LightDotted1, DLightRoundedOnward

		#define DLightOnwardNoOpaque \
			ETerminalEndpointType::mc_LightDotted3, ETerminalEndpointType::mc_LightDotted2, \
			ETerminalEndpointType::mc_LightDotted1, DLightRoundedOnwardNoOpaque

		#define DLightNoRounded ETerminalEndpointType::mc_Light, \
			ETerminalEndpointType::mc_LightDotted3, ETerminalEndpointType::mc_LightDotted2, \
			ETerminalEndpointType::mc_LightDotted1

		#define DDoubleOnward ETerminalEndpointType::mc_Double, DLightOnward
		#define DDoubleOnwardNoOpaque ETerminalEndpointType::mc_Double, DLightOnwardNoOpaque

		#define DHeavyDotted1Onward ETerminalEndpointType::mc_HeavyDotted1, DDoubleOnward
		#define DHeavyDotted2Onward ETerminalEndpointType::mc_HeavyDotted2, DHeavyDotted1Onward
		#define DHeavyDotted3Onward ETerminalEndpointType::mc_HeavyDotted3, DHeavyDotted2Onward

		#define DHeavyOnward ETerminalEndpointType::mc_Heavy, DHeavyDotted3Onward

		#define DAllHeavy ETerminalEndpointType::mc_Heavy, ETerminalEndpointType::mc_HeavyDotted3, \
			ETerminalEndpointType::mc_HeavyDotted2, ETerminalEndpointType::mc_HeavyDotted1

		#define DAllHeavyAndDouble ETerminalEndpointType::mc_Heavy, ETerminalEndpointType::mc_HeavyDotted3, \
			ETerminalEndpointType::mc_HeavyDotted2, ETerminalEndpointType::mc_HeavyDotted1, ETerminalEndpointType::mc_Double

		constexpr CBoxMergeSource gc_BoxMergesSources[] =
			{
				#include "BoxMerges/Malterlib_CommandLine_BoxMerges_BarHorizontalHeavy.hpp"
				#include "BoxMerges/Malterlib_CommandLine_BoxMerges_BarHorizontalDouble.hpp"
				#include "BoxMerges/Malterlib_CommandLine_BoxMerges_BarHorizontalLight.hpp"
				#include "BoxMerges/Malterlib_CommandLine_BoxMerges_BarVerticalHeavy.hpp"
				#include "BoxMerges/Malterlib_CommandLine_BoxMerges_BarVerticalDouble.hpp"
				#include "BoxMerges/Malterlib_CommandLine_BoxMerges_BarVerticalLight.hpp"
				#include "BoxMerges/Malterlib_CommandLine_BoxMerges_Coupling.hpp"
				#include "BoxMerges/Malterlib_CommandLine_BoxMerges_CrossHorizontalDouble.hpp"
				#include "BoxMerges/Malterlib_CommandLine_BoxMerges_CrossVerticalDouble.hpp"
				#include "BoxMerges/Malterlib_CommandLine_BoxMerges_CrossFullDouble.hpp"
				#include "BoxMerges/Malterlib_CommandLine_BoxMerges_CrossFullHeavy.hpp"
				#include "BoxMerges/Malterlib_CommandLine_BoxMerges_CrossFullLight.hpp"
				#include "BoxMerges/Malterlib_CommandLine_BoxMerges_CrossBottomLeftHeavy.hpp"
				#include "BoxMerges/Malterlib_CommandLine_BoxMerges_CrossBottomRightHeavy.hpp"
				#include "BoxMerges/Malterlib_CommandLine_BoxMerges_CrossDownHeavy.hpp"
				#include "BoxMerges/Malterlib_CommandLine_BoxMerges_CrossHorizontalHeavy.hpp"
				#include "BoxMerges/Malterlib_CommandLine_BoxMerges_CrossLeftHeavy.hpp"
				#include "BoxMerges/Malterlib_CommandLine_BoxMerges_CrossRightHeavy.hpp"
				#include "BoxMerges/Malterlib_CommandLine_BoxMerges_CrossTopLeftHeavy.hpp"
				#include "BoxMerges/Malterlib_CommandLine_BoxMerges_CrossTopRightHeavy.hpp"
				#include "BoxMerges/Malterlib_CommandLine_BoxMerges_CrossUpHeavy.hpp"
				#include "BoxMerges/Malterlib_CommandLine_BoxMerges_CrossVerticalHeavy.hpp"
				#include "BoxMerges/Malterlib_CommandLine_BoxMerges_CrossDownLight.hpp"
				#include "BoxMerges/Malterlib_CommandLine_BoxMerges_CrossLeftLight.hpp"
				#include "BoxMerges/Malterlib_CommandLine_BoxMerges_CrossRightLight.hpp"
				#include "BoxMerges/Malterlib_CommandLine_BoxMerges_CrossUpLight.hpp"
				#include "BoxMerges/Malterlib_CommandLine_BoxMerges_ElbowTopLeftHeavy.hpp"
				#include "BoxMerges/Malterlib_CommandLine_BoxMerges_ElbowTopLeftDouble.hpp"
				#include "BoxMerges/Malterlib_CommandLine_BoxMerges_ElbowTopLeftLight.hpp"
				#include "BoxMerges/Malterlib_CommandLine_BoxMerges_ElbowTopRightHeavy.hpp"
				#include "BoxMerges/Malterlib_CommandLine_BoxMerges_ElbowTopRightDouble.hpp"
				#include "BoxMerges/Malterlib_CommandLine_BoxMerges_ElbowTopRightLight.hpp"
				#include "BoxMerges/Malterlib_CommandLine_BoxMerges_ElbowBottomLeftHeavy.hpp"
				#include "BoxMerges/Malterlib_CommandLine_BoxMerges_ElbowBottomLeftDouble.hpp"
				#include "BoxMerges/Malterlib_CommandLine_BoxMerges_ElbowBottomLeftLight.hpp"
				#include "BoxMerges/Malterlib_CommandLine_BoxMerges_ElbowBottomRightHeavy.hpp"
				#include "BoxMerges/Malterlib_CommandLine_BoxMerges_ElbowBottomRightDouble.hpp"
				#include "BoxMerges/Malterlib_CommandLine_BoxMerges_ElbowBottomRightLight.hpp"
				#include "BoxMerges/Malterlib_CommandLine_BoxMerges_EndingLeft.hpp"
				#include "BoxMerges/Malterlib_CommandLine_BoxMerges_EndingRight.hpp"
				#include "BoxMerges/Malterlib_CommandLine_BoxMerges_EndingTop.hpp"
				#include "BoxMerges/Malterlib_CommandLine_BoxMerges_EndingBottom.hpp"
				#include "BoxMerges/Malterlib_CommandLine_BoxMerges_TeeDownHeavy.hpp"
				#include "BoxMerges/Malterlib_CommandLine_BoxMerges_TeeLeftHeavy.hpp"
				#include "BoxMerges/Malterlib_CommandLine_BoxMerges_TeeRightHeavy.hpp"
				#include "BoxMerges/Malterlib_CommandLine_BoxMerges_TeeUpHeavy.hpp"
				#include "BoxMerges/Malterlib_CommandLine_BoxMerges_TeeDownDouble.hpp"
				#include "BoxMerges/Malterlib_CommandLine_BoxMerges_TeeLeftDouble.hpp"
				#include "BoxMerges/Malterlib_CommandLine_BoxMerges_TeeRightDouble.hpp"
				#include "BoxMerges/Malterlib_CommandLine_BoxMerges_TeeUpDouble.hpp"
				#include "BoxMerges/Malterlib_CommandLine_BoxMerges_TeeDownLight.hpp"
				#include "BoxMerges/Malterlib_CommandLine_BoxMerges_TeeLeftLight.hpp"
				#include "BoxMerges/Malterlib_CommandLine_BoxMerges_TeeRightLight.hpp"
				#include "BoxMerges/Malterlib_CommandLine_BoxMerges_TeeUpLight.hpp"
			}
		;

		#undef DInsertFileLine
		#undef DLightRoundedOnward
		#undef DLightRoundedOnwardNoOpaque
		#undef DLightAllDotted3Onward
		#undef DLightAllDotted2Onward
		#undef DLightAllDotted1Onward
		#undef DLightOnward
		#undef DLightOnwardNoOpaque
		#undef DLightNoRounded
		#undef DDoubleOnward
		#undef DDoubleOnwardNoOpaque
		#undef DHeavyDotted3Onward
		#undef DHeavyDotted2Onward
		#undef DHeavyDotted1Onward
		#undef DHeavyOnward
		#undef DAllHeavy
		#undef DAllHeavyAndDouble
#endif
		struct CCharacterPair
		{
			COrdering_Strong operator <=> (CCharacterPair const &_Right) const
			{
				if (auto Compare = m_Left <=> _Right.m_Left; Compare != 0)
					return Compare;

				return m_Right <=> _Right.m_Right;
			}

			bool operator == (CCharacterPair const &_Right) const
			{
				if (m_Left != _Right.m_Left)
					return false;

				return m_Right == _Right.m_Right;
			}

			void f_MakeCanonical()
			{
				if (m_Left > m_Right)
					fg_Swap(m_Left, m_Right);
			}

			ch32 m_Left = 0;
			ch32 m_Right = 0;
		};

		struct CBoxMerge
		{
			COrdering_Strong operator <=>(CBoxMerge const &_Right) const
			{
				return m_Characters <=> _Right.m_Characters;
			}

			COrdering_Strong operator <=>(CCharacterPair const &_Right) const
			{
				return m_Characters <=> _Right;
			}

			CCharacterPair m_Characters;
			ch32 m_Combined = 0;

#if DCalculateBoxMerges
			ch8 const *m_pFile = nullptr;
			int32 m_Line = 0;
#endif
		};
	}
}

#include "Malterlib_CommandLine_Terminal_Surface_BoxMergesTable.hpp"

namespace NMib::NCommandLine
{
	namespace
	{
#if DCalculateBoxMerges
		static CUStr fg_FormatChar(ch32 _CharacterAndEndpointType)
		{
			ch32 Character = _CharacterAndEndpointType & CChixel::mc_CharacterMask;
			ch32 CharStr[] = {Character, 0};
			auto EndpointType = ETerminalEndpointType((_CharacterAndEndpointType & CChixel::mc_EndpointMask) >> CChixel::mc_EndpointOffset);
			switch (EndpointType)
			{
			case ETerminalEndpointType::mc_Unspecified: return CharStr;
			case ETerminalEndpointType::mc_Heavy: return CUStr::CFormat(str_utf32("{} (Heavy)")) << CharStr;
			case ETerminalEndpointType::mc_HeavyDotted1: return CUStr::CFormat(str_utf32("{} (HeavyDotted1)")) << CharStr;
			case ETerminalEndpointType::mc_HeavyDotted2: return CUStr::CFormat(str_utf32("{} (HeavyDotted2)")) << CharStr;
			case ETerminalEndpointType::mc_HeavyDotted3: return CUStr::CFormat(str_utf32("{} (HeavyDotted3)")) << CharStr;
			case ETerminalEndpointType::mc_Double: return CUStr::CFormat(str_utf32("{} (Double)")) << CharStr;
			case ETerminalEndpointType::mc_Light: return CUStr::CFormat(str_utf32("{} (Light)")) << CharStr;
			case ETerminalEndpointType::mc_LightDotted1: return CUStr::CFormat(str_utf32("{} (LightDotted1)")) << CharStr;
			case ETerminalEndpointType::mc_LightDotted2: return CUStr::CFormat(str_utf32("{} (LightDotted2)")) << CharStr;
			case ETerminalEndpointType::mc_LightDotted3: return CUStr::CFormat(str_utf32("{} (LightDotted3)")) << CharStr;
			case ETerminalEndpointType::mc_LightRounded: return CUStr::CFormat(str_utf32("{} (LightRounded)")) << CharStr;
			case ETerminalEndpointType::mc_LightDotted1Rounded: return CUStr::CFormat(str_utf32("{} (LightDotted1Rounded)")) << CharStr;
			case ETerminalEndpointType::mc_LightDotted2Rounded: return CUStr::CFormat(str_utf32("{} (LightDotted2Rounded)")) << CharStr;
			case ETerminalEndpointType::mc_LightDotted3Rounded: return CUStr::CFormat(str_utf32("{} (LightDotted3Rounded)")) << CharStr;
			}

			return CharStr;
		}
#endif
		struct CBoxMergesSorted
		{
#if DCalculateBoxMerges
			CBoxMergesSorted()
			{
				NContainer::TCMap<CCharacterPair, ch32> MappedCharacters;

				for (auto &Source : gc_BoxMergesSources)
				{
					NContainer::TCMap<ch32, NContainer::TCSet<ch32>> Same;
					bool bFirst = true;
					ch32 Last;
					for (auto iSame = Source.m_Same.f_GetUnicodeIterator(); iSame; ++iSame)
					{
						if (bFirst)
							Last = *iSame;
						else
						{
							Same[Last][*iSame];
							Same[*iSame][Last];
						}

						bFirst = !bFirst;
					}

					auto fIsValidEndpointType = [](ch32 _Char, ETerminalEndpointType _EndpointType)
						{
							if (fg_StrFindChar(str_utf32("╸╹╺╻"), _Char) >= 0)
							{
								switch (_EndpointType)
								{
								case ETerminalEndpointType::mc_Heavy:
								case ETerminalEndpointType::mc_HeavyDotted1:
								case ETerminalEndpointType::mc_HeavyDotted2:
								case ETerminalEndpointType::mc_HeavyDotted3:
								case ETerminalEndpointType::mc_Double:
									return true;
								default:
								}
								return false;
							}
							else if (fg_StrFindChar(str_utf32("╴╵╶╷"), _Char) >= 0)
							{
								switch (_EndpointType)
								{
								case ETerminalEndpointType::mc_Light:
								case ETerminalEndpointType::mc_LightDotted1:
								case ETerminalEndpointType::mc_LightDotted2:
								case ETerminalEndpointType::mc_LightDotted3:
								case ETerminalEndpointType::mc_LightRounded:
								case ETerminalEndpointType::mc_LightDotted1Rounded:
								case ETerminalEndpointType::mc_LightDotted2Rounded:
								case ETerminalEndpointType::mc_LightDotted3Rounded:
									return true;
								default:
								}
								return false;
							}
							else if (_EndpointType != ETerminalEndpointType::mc_Unspecified)
								return false;

							return true;
						}
					;

					NContainer::TCSet<CCharacterPair> AddedPairs;
					for (auto iLeft = Source.m_Left.f_GetUnicodeIterator(); iLeft; ++iLeft)
					{
						for (auto iRight = Source.m_Right.f_GetUnicodeIterator(); iRight; ++iRight)
						{
							auto CharLeft = *iLeft;
							auto CharRight = *iRight;

							bool bFoundUnspecifiedLeft = false;
							bool bFoundAny = false;

							for (auto &EndpointTypeLeft : Source.m_EndpointTypesLeft)
							{
								if (EndpointTypeLeft == ETerminalEndpointType::mc_Unspecified)
									bFoundUnspecifiedLeft = true;

								if (!fIsValidEndpointType(CharLeft, EndpointTypeLeft))
									continue;

								bool bFoundUnspecifiedRight = false;

								for (auto &EndpointTypeRight : Source.m_EndpointTypesRight)
								{
									if (EndpointTypeRight == ETerminalEndpointType::mc_Unspecified)
										bFoundUnspecifiedRight = true;

									if (!fIsValidEndpointType(CharRight, EndpointTypeRight))
										continue;

									bFoundAny = true;

									if (auto *pSame = Same.f_FindEqual(CharLeft); pSame && pSame->f_FindEqual(CharRight))
										continue;

									CCharacterPair ToAdd{.m_Left = fg_CharacterAndEndpointType(CharLeft, EndpointTypeLeft), .m_Right = fg_CharacterAndEndpointType(CharRight, EndpointTypeRight)};

									if (ToAdd.m_Left == ToAdd.m_Right)
										continue;

									ToAdd.f_MakeCanonical();

									if (!AddedPairs(ToAdd).f_WasCreated())
										continue;

									auto Mapping = MappedCharacters(ToAdd, Source.m_Combined);
									if (!Mapping.f_WasCreated())
									{
										if (*Mapping == Source.m_Combined)
											continue;
									}

									m_BoxMerges.f_Insert(CBoxMerge{.m_Characters = ToAdd, .m_Combined = Source.m_Combined, .m_Line = Source.m_Line, .m_pFile = Source.m_pFile});

									if (EndpointTypeRight == ETerminalEndpointType::mc_Unspecified)
										break;
								}

								DMibCheck(bFoundUnspecifiedRight);

								if (EndpointTypeLeft == ETerminalEndpointType::mc_Unspecified)
									break;
							}

							if (!bFoundAny)
							{
								DMibConOut
									(
										"Unused Charater Tuple: Left: {} Right: {}\n"
										DMibPFileLineFormat " {}\n"
										, fg_FormatChar(CharLeft)
										, fg_FormatChar(CharRight)
										, Source.m_pFile
										, Source.m_Line
										, fg_FormatChar(Source.m_Combined)
									)
								;
								DMibPDebugBreak;
							}

							DMibCheck(bFoundUnspecifiedLeft);
						}
					}
				}

				DMibConOut("m_BoxMerges.f_GetLen(): {}\n", m_BoxMerges.f_GetLen());

				for (auto &BoxMerge : m_BoxMerges)
					BoxMerge.m_Characters.f_MakeCanonical();

				m_BoxMerges.f_Sort();

				NContainer::TCMap<ch32, NContainer::TCMap<ch32, NContainer::TCVector<ch32>>> MappedPerDestination;
				for (auto &BoxMerge : m_BoxMerges)
				{
					auto &Dest = MappedPerDestination[BoxMerge.m_Combined][BoxMerge.m_Characters.m_Left];
					Dest.f_Insert(BoxMerge.m_Characters.m_Right);
				}

				for (auto &Dest : MappedPerDestination.f_Entries())
				{
					for (auto &LeftRight : Dest.f_Value().f_Entries())
					{
						LeftRight.f_Value().f_Sort();
						CUStr Temp;
						for (auto &Char : LeftRight.f_Value())
						{
							if (!Temp.f_IsEmpty())
								Temp.f_AddChar(' ');
							Temp += fg_FormatChar(Char);
						}
					}
				}

				auto Last = m_BoxMerges.f_GetFirst();
				bool bFirst = true;
				for (auto &BoxMerge : m_BoxMerges)
				{
					if (!bFirst && BoxMerge.m_Characters == Last.m_Characters)
					{
						DMibConOut
							(
								"Duplicate: Left: {} Right: {}\n"
								DMibPFileLineFormat " {}\n"
								DMibPFileLineFormat " {}\n"
								, fg_FormatChar(BoxMerge.m_Characters.m_Left)
								, fg_FormatChar(BoxMerge.m_Characters.m_Right)
								, BoxMerge.m_pFile
								, BoxMerge.m_Line
								, fg_FormatChar(BoxMerge.m_Combined)
								, Last.m_pFile
								, Last.m_Line
								, fg_FormatChar(Last.m_Combined)
							)
						;
						DMibPDebugBreak;
					}

					bFirst = false;
					Last = BoxMerge;
				}

				DMibConOut("{sf\n,sj104}", "");

				CUStr AllCharacters;

				for (auto Char : str_utf32("─━│┃┄┅┆┇┈┉┊┋┌┍┎┏┐┑┒┓└┕┖┗┘┙┚┛├┝┞┟┠┡┢┣┤┥┦┧┨┩┪┫┬┭┮┯┰┱┲┳┴┵┶┷┸┹┺┻┼┽┾┿╀╁╂╃╄╅╆╇╈╉╊╋╌╍╎╏═║╒╓╔╕╖╗╘╙╚╛╜╝╞╟╠╡╢╣╤╥╦╧╨╩╪╫╬╭╮╯╰╼╽╾╿"))
				{
					if (!Char)
						continue;

					AllCharacters.f_AddChar(Char);
				}

				for (auto Char : str_utf32("╴╵╶╷"))
				{
					if (!Char)
						continue;

					for
						(
							auto Type :
							{
								ETerminalEndpointType::mc_Light
								, ETerminalEndpointType::mc_LightDotted1
								, ETerminalEndpointType::mc_LightDotted2
								, ETerminalEndpointType::mc_LightDotted3
								, ETerminalEndpointType::mc_LightRounded
								, ETerminalEndpointType::mc_LightDotted1Rounded
								, ETerminalEndpointType::mc_LightDotted2Rounded
								, ETerminalEndpointType::mc_LightDotted3Rounded
							}
						)
					{
						AllCharacters.f_AddChar(fg_CharacterAndEndpointType(Char, Type));
					}
				}

				for (auto Char : str_utf32("╸╹╺╻"))
				{
					if (!Char)
						continue;

					for
						(
							auto Type :
							{
								ETerminalEndpointType::mc_Heavy
								, ETerminalEndpointType::mc_Double
								, ETerminalEndpointType::mc_HeavyDotted1
								, ETerminalEndpointType::mc_HeavyDotted2
								, ETerminalEndpointType::mc_HeavyDotted3
							}
						)
					{
						AllCharacters.f_AddChar(fg_CharacterAndEndpointType(Char, Type));
					}
				}

				NContainer::TCSet<CCharacterPair> MissingCharacters;

				for (auto CharLeft : AllCharacters.f_GetUnicodeIterator())
				{
					for (auto CharRight : AllCharacters.f_GetUnicodeIterator())
					{
						if (CharLeft == CharRight)
							continue;

						CCharacterPair Pair{.m_Left = CharLeft, .m_Right = CharRight};
						Pair.f_MakeCanonical();

						if (!MappedCharacters.f_FindEqual(Pair))
							MissingCharacters[Pair];

					}
				}

				if (!MissingCharacters.f_IsEmpty())
				{
					for (auto &MissingPair : MissingCharacters)
						DMibConOut("{} + {}\n\n", fg_FormatChar(MissingPair.m_Left), fg_FormatChar(MissingPair.m_Right));

					DMibConOut("nMissing: {}\n", MissingCharacters.f_GetLen());
				}

#if DOutputBoxMerges
				for (auto &BoxMerge : m_BoxMerges)
					DMibConOut(", {{{{{}, {}}, {}}\n", BoxMerge.m_Characters.m_Left, BoxMerge.m_Characters.m_Right, BoxMerge.m_Combined);
				NSys::fg_TerminateProcess(1);
#endif
			}
#endif

			ch32 f_GetCombined(ch32 _Left, ch32 _Right) const
			{
				CCharacterPair CharacterPair{.m_Left = _Left, .m_Right = _Right};
				CharacterPair.f_MakeCanonical();

#if DCalculateBoxMerges
				auto iBoxMerge = m_BoxMerges.f_BinarySearch(CharacterPair);
				if (iBoxMerge >= 0)
					return m_BoxMerges.f_GetArray()[iBoxMerge].m_Combined;

				return 0;
#else
				constexpr umint c_Len = sizeof(gc_BoxMergesTable) / sizeof(gc_BoxMergesTable[0]);
				umint Low = 0;
				umint High = c_Len;

				while (Low < High)
				{
					umint Mid = (Low + High) >> 1;
					if ((gc_BoxMergesTable[Mid].m_Characters <=> CharacterPair) < 0)
						Low = Mid + 1;
					else
						High = Mid;
				}

				if (Low < c_Len && (gc_BoxMergesTable[Low].m_Characters <=> CharacterPair) == 0)
					return gc_BoxMergesTable[Low].m_Combined;
				else
					return 0;
#endif
			}

#if DCalculateBoxMerges
			NContainer::TCVector<CBoxMerge> m_BoxMerges;
#endif
		};
	}
}
