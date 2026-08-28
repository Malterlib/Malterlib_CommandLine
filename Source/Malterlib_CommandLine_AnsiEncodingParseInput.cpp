// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "Malterlib_CommandLine_AnsiEncodingParseInput.h"

namespace NMib::NCommandLine
{
	using namespace NMib::NStr;

	// https://invisible-island.net/xterm/ctlseqs/ctlseqs.html
	// https://sw.kovidgoyal.net/kitty/keyboard-protocol/

	namespace
	{
		EKeyModifier fg_DecodeModifiers(int32 _Param)
		{
			if (_Param < 1)
				return EKeyModifier::mc_None;

			return EKeyModifier(uint8(_Param - 1));
		}

		ch32 fg_TildeKeyCode(int32 _Number)
		{
			switch (_Number)
			{
				case 1: return ch32(EKey::mc_Home); // Linux console and vt220 style Home
				case 2: return ch32(EKey::mc_Insert);
				case 3: return ch32(EKey::mc_Delete);
				case 4: return ch32(EKey::mc_End); // Linux console and vt220 style End
				case 5: return ch32(EKey::mc_PageUp);
				case 6: return ch32(EKey::mc_PageDown);
				case 7: return ch32(EKey::mc_Home);
				case 8: return ch32(EKey::mc_End);
				case 11: return ch32(EKey::mc_F1);
				case 12: return ch32(EKey::mc_F2);
				case 13: return ch32(EKey::mc_F3);
				case 14: return ch32(EKey::mc_F4);
				case 15: return ch32(EKey::mc_F5);
				case 17: return ch32(EKey::mc_F6);
				case 18: return ch32(EKey::mc_F7);
				case 19: return ch32(EKey::mc_F8);
				case 20: return ch32(EKey::mc_F9);
				case 21: return ch32(EKey::mc_F10);
				case 23: return ch32(EKey::mc_F11);
				case 24: return ch32(EKey::mc_F12);
				default: return 0;
			}
		}

		ch32 fg_LetterKeyCode(char _Final)
		{
			switch (_Final)
			{
				case 'A': return ch32(EKey::mc_Up);
				case 'B': return ch32(EKey::mc_Down);
				case 'C': return ch32(EKey::mc_Right);
				case 'D': return ch32(EKey::mc_Left);
				case 'H': return ch32(EKey::mc_Home);
				case 'F': return ch32(EKey::mc_End);
				case 'P': return ch32(EKey::mc_F1);
				case 'Q': return ch32(EKey::mc_F2);
				case 'R': return ch32(EKey::mc_F3);
				case 'S': return ch32(EKey::mc_F4);
				default: return 0;
			}
		}

		// Splits _Param into separator delimited views of the input bytes (empty pieces are kept,
		// matching the previous split based parsing); at most t_MaxPieces are stored while the full
		// count is returned
		template <umint t_MaxPieces>
		umint fg_SplitPieces(CStrPtr const &_Param, ch8 _Separator, CStrPtr (&o_Pieces)[t_MaxPieces])
		{
			umint nPieces = 0;

			ch8 const *pPos = _Param.f_GetStr();
			ch8 const *pEnd = pPos + _Param.f_GetLen();
			for (;;)
			{
				ch8 const *pSeparator = pPos;
				while (pSeparator < pEnd && *pSeparator != _Separator)
					++pSeparator;

				if (nPieces < t_MaxPieces)
					o_Pieces[nPieces] = CStrPtr(pPos, pSeparator - pPos);
				++nPieces;

				if (pSeparator == pEnd)
					break;

				pPos = pSeparator + 1;
			}

			return nPieces;
		}
	}

	CAnsiEncodingParseInput::CAnsiEncodingParseInput(CAnsiEncodingParseInputOptions &&_Options)
		: mp_Options(fg_Move(_Options))
	{
	}

	void CAnsiEncodingParseInput::fp_EmitKey(CKeyEvent &&_KeyEvent)
	{
		if (mp_Options.m_fOnKeyEvent)
			mp_Options.m_fOnKeyEvent(fg_Move(_KeyEvent));
	}

	void CAnsiEncodingParseInput::fp_EmitControl(ch8 _Char)
	{
		CKeyEvent Event;

		switch (_Char)
		{
			case 0x00:
				Event.m_ScanCode = ch32(EKey::mc_Space);
				Event.m_Modifiers = EKeyModifier::mc_Ctrl;
				break;

			case 0x08:
				Event.m_ScanCode = ch32(EKey::mc_Backspace);
				Event.m_Modifiers = EKeyModifier::mc_Ctrl;
				break;

			case 0x09:
				Event.m_ScanCode = ch32(EKey::mc_Tab);
				break;

			case 0x0A:
			case 0x0D:
				Event.m_ScanCode = ch32(EKey::mc_Enter);
				break;

			case 0x7F:
				Event.m_ScanCode = ch32(EKey::mc_Backspace);
				break;

			default:
				if (_Char >= 0x01 && _Char <= 0x1A)
				{
					Event.m_ScanCode = ch32('a' + _Char - 1);
					Event.m_Modifiers = EKeyModifier::mc_Ctrl;
				}
				else if (_Char >= 0x1C && _Char <= 0x1F)
				{
					// Traditional C0 controls for Ctrl with the printable characters \ ] ^ _
					Event.m_ScanCode = ch32('\\' + _Char - 0x1C);
					Event.m_Modifiers = EKeyModifier::mc_Ctrl;
				}
				else
				{
					Event.m_ScanCode = ch32(uch8(_Char));
					Event.m_Modifiers = EKeyModifier::mc_Ctrl;
				}
				break;
		}

		fp_EmitKey(fg_Move(Event));
	}

	void CAnsiEncodingParseInput::fp_DispatchCsiMouse(CStrPtr const &_Params, char _Final)
	{
		// SGR mouse report: CSI < Btn ; X ; Y M (press/move) or m (release)
		CStrPtr Params[3];
		umint nParams = fg_SplitPieces(CStrPtr(_Params.f_GetStr() + 1, _Params.f_GetLen() - 1), ';', Params);
		if (nParams < 3)
			return;

		int32 ButtonBits = Params[0].f_ToInt(0);
		int32 CellX = Params[1].f_ToInt(1) - 1;
		int32 CellY = Params[2].f_ToInt(1) - 1;

		CTerminalMouseEvent Event;
		Event.m_CoordX = CellX;
		Event.m_CoordY = CellY;

		if (ButtonBits & 0x04)
			Event.m_Modifiers |= EKeyModifier::mc_Shift;
		if (ButtonBits & 0x08)
			Event.m_Modifiers |= EKeyModifier::mc_Alt;
		if (ButtonBits & 0x10)
			Event.m_Modifiers |= EKeyModifier::mc_Ctrl;

		if (ButtonBits & 0x40)
		{
			Event.m_EventType = EMouseEventType::mc_Wheel;

			// Wheel buttons: 64 up, 65 down, 66 left, 67 right
			switch (ButtonBits & 0x03)
			{
				case 0:
					Event.m_WheelDelta = 1.0;
					break;

				case 1:
					Event.m_WheelDelta = -1.0;
					break;

				case 2:
					Event.m_WheelDeltaX = 1.0;
					break;

				case 3:
					Event.m_WheelDeltaX = -1.0;
					break;
			}
		}
		else
		{
			switch (ButtonBits & 0x03)
			{
				case 0:
					Event.m_Button = EMouseButton::mc_Left;
					break;

				case 1:
					Event.m_Button = EMouseButton::mc_Middle;
					break;

				case 2:
					Event.m_Button = EMouseButton::mc_Right;
					break;

				default:
					Event.m_Button = EMouseButton::mc_None;
					break;
			}

			if (ButtonBits & 0x20)
				Event.m_EventType = EMouseEventType::mc_Move;
			else if (_Final == 'm')
				Event.m_EventType = EMouseEventType::mc_Up;
			else
				Event.m_EventType = EMouseEventType::mc_Down;
		}

		if (mp_Options.m_fOnMouseEvent)
			mp_Options.m_fOnMouseEvent(fg_Move(Event));
	}

	void CAnsiEncodingParseInput::fp_DispatchCsi(CStrPtr const &_Params, char _Final)
	{
		if (_Params.f_StartsWith("<"))
		{
			if (_Final == 'M' || _Final == 'm')
				fp_DispatchCsiMouse(_Params, _Final);

			return;
		}

		if (_Params.f_StartsWith("?"))
		{
			// Reply to the comprehensive keyboard support query: CSI ? flags u
			if (_Final == 'u' && mp_Options.m_fOnComprehensiveKeySupport)
			{
				CStrPtr Flags(_Params.f_GetStr() + 1, _Params.f_GetLen() - 1);

				mp_Options.m_fOnComprehensiveKeySupport(uint8(Flags.f_ToInt(0)));
			}

			return;
		}

		constexpr umint c_MaxParams = 8;
		CStrPtr Params[c_MaxParams];
		umint nParams = fg_SplitPieces(_Params, ';', Params);
		auto fGetParam = [&](umint _iParam, int32 _Default) -> int32
			{
				if (_iParam >= nParams || _iParam >= c_MaxParams)
					return _Default;

				return Params[_iParam].f_ToInt(_Default, ":");
			}
		;

		// The modifiers[:event] subparameter is shared by every key form the terminal emits, the
		// legacy final letter forms included (CSI 1;1:3 A is an arrow release)
		auto fDecodeEventType = [&](CKeyEvent &_Event)
			{
				if (nParams < 2)
					return;

				CStrPtr ModifierParts[2];
				umint nModifierParts = fg_SplitPieces(Params[1], ':', ModifierParts);
				if (nModifierParts < 2)
					return;

				switch (ModifierParts[1].f_ToInt(1))
				{
					case 2:
						_Event.m_EventType = EKeyEventType::mc_Repeat;
						break;

					case 3:
						_Event.m_EventType = EKeyEventType::mc_Release;
						break;

					default:
						break;
				}
			}
		;

		switch (_Final)
		{
			case 'u':
			{
				// kitty comprehensive key: CSI unicode[:shifted[:base]] ; modifiers[:event] ; text
				// [; report-id] u — the fourth parameter is the key event handling report id
				// extension
				CKeyEvent Event;
				Event.m_ScanCode = ch32(fGetParam(0, 0));
				Event.m_Modifiers = fg_DecodeModifiers(fGetParam(1, 1));
				Event.m_HandlingReportID = uint16(fGetParam(3, 0));

				// With the handling report extension active the functional keys that only have
				// legacy sequences arrive in the extension's Private Use Area block: Insert
				// through End, then F1 through F12, in EKey order
				if (Event.m_ScanCode >= 0xF500 && Event.m_ScanCode <= 0xF515)
				{
					ch32 Offset = Event.m_ScanCode - 0xF500;
					Event.m_ScanCode = Offset < 10 ? ch32(EKey::mc_Insert) + Offset : ch32(EKey::mc_F1) + (Offset - 10);
				}

				fDecodeEventType(Event);

				// An empty text field is one left out ahead of the report id, such as CSI 99;1;;42 u
				// for a deferred printable key, and the text is then synthesized below as when the
				// field is absent
				if (nParams >= 3 && !Params[2].f_IsEmpty())
				{
					ch8 const *pPos = Params[2].f_GetStr();
					ch8 const *pEnd = pPos + Params[2].f_GetLen();
					for (;;)
					{
						ch8 const *pSeparator = pPos;
						while (pSeparator < pEnd && *pSeparator != ':')
							++pSeparator;

						Event.m_Text += fg_CharToString(ch32(CStrPtr(pPos, pSeparator - pPos).f_ToInt(0)));

						if (pSeparator == pEnd)
							break;

						pPos = pSeparator + 1;
					}
				}
				else if
				(
					Event.m_EventType != EKeyEventType::mc_Release
					&& Event.m_ScanCode >= 0x20
					&& Event.m_ScanCode != ch32(EKey::mc_Backspace)
					&& (Event.m_ScanCode < 57344 || Event.m_ScanCode > 63743) // Kitty functional key range
					&& !fg_IsSet(Event.m_Modifiers, (EKeyModifier::mc_Ctrl | EKeyModifier::mc_Super | EKeyModifier::mc_Hyper | EKeyModifier::mc_Meta))
				)
				{
					// Without the optional associated text parameter the codepoint itself is the
					// text — but only for unmodified (or shift/alt) strokes, and only for a press or
					// repeat: a chord like ctrl+t does not type a 't', and a release types nothing.
					// A shifted stroke types its shifted variant: the one the terminal reported in
					// the key's second subfield, or the upper case letter when it reported none
					ch32 TextCode = Event.m_ScanCode;
					if (fg_IsSet(Event.m_Modifiers, EKeyModifier::mc_Shift))
					{
						CStrPtr KeyParts[3];
						umint nKeyParts = fg_SplitPieces(Params[0], ':', KeyParts);
						if (nKeyParts >= 2 && !KeyParts[1].f_IsEmpty())
							TextCode = ch32(KeyParts[1].f_ToInt(0));
						else if (TextCode >= 'a' && TextCode <= 'z')
							TextCode = TextCode - 'a' + 'A';
					}
					Event.m_Text = fg_CharToString(TextCode);
				}

				fp_EmitKey(fg_Move(Event));

				break;
			}

			case '~':
			{
				ch32 ScanCode = fg_TildeKeyCode(fGetParam(0, 0));
				if (!ScanCode)
					break;

				CKeyEvent Event;
				Event.m_ScanCode = ScanCode;
				Event.m_Modifiers = fg_DecodeModifiers(fGetParam(1, 1));

				fDecodeEventType(Event);

				fp_EmitKey(fg_Move(Event));

				break;
			}

			case 'Z':
			{
				CKeyEvent Event;
				Event.m_ScanCode = ch32(EKey::mc_Tab);
				Event.m_Modifiers = fg_DecodeModifiers(fGetParam(1, 1)) | EKeyModifier::mc_Shift;

				fDecodeEventType(Event);

				fp_EmitKey(fg_Move(Event));

				break;
			}

			default:
			{
				ch32 ScanCode = fg_LetterKeyCode(_Final);
				if (!ScanCode)
				{
					DMibLog(Debug, "Unknown CSI sequence: {} '{}'", _Params, fg_CharToString(_Final));

					break;
				}

				CKeyEvent Event;
				Event.m_ScanCode = ScanCode;
				Event.m_Modifiers = fg_DecodeModifiers(fGetParam(1, 1));

				fDecodeEventType(Event);

				fp_EmitKey(fg_Move(Event));

				break;
			}
		}
	}

	bool CAnsiEncodingParseInput::fp_ParseCsi(ch8 const *&_pParse, ch8 const *_pEnd)
	{
		// _pParse points after "ESC ["; returns false if the sequence is incomplete
		ch8 const *pParse = _pParse;

		// The Linux console emits F1 to F5 as ESC [ [ A to ESC [ [ E
		if (pParse < _pEnd && *pParse == '[')
		{
			++pParse;
			if (pParse >= _pEnd)
				return false;

			ch8 Letter = *pParse;
			++pParse;

			if (Letter >= 'A' && Letter <= 'E')
			{
				CKeyEvent Event;
				Event.m_ScanCode = ch32(EKey::mc_F1) + ch32(Letter - 'A');
				fp_EmitKey(fg_Move(Event));
			}

			_pParse = pParse;

			return true;
		}

		ch8 const *pParamsStart = pParse;
		while (pParse < _pEnd && *pParse >= 0x30 && *pParse <= 0x3F)
			++pParse;
		CStrPtr ParamsStr(pParamsStart, pParse - pParamsStart);

		while (pParse < _pEnd && *pParse >= 0x20 && *pParse <= 0x2F)
			++pParse;

		if (pParse >= _pEnd)
			return false;

		char Final = *pParse;
		++pParse;

		fp_DispatchCsi(ParamsStr, Final);

		_pParse = pParse;

		return true;
	}

	void CAnsiEncodingParseInput::f_AddInput(CStr const &_Input)
	{
		mp_InputBuffer += _Input;

		ch8 const *pStartParse = mp_InputBuffer.f_GetStr();
		ch8 const *pEnd = pStartParse + mp_InputBuffer.f_GetLen();
		ch8 const *pParse = pStartParse;
		ch8 const *pFinishedParse = pParse;

		while (pParse < pEnd)
		{
			ch8 Char = *pParse;

			if (Char == '\x1B')
			{
				ch8 const *pSequence = pParse + 1;

				if (pSequence >= pEnd)
				{
					// A trailing escape can be either the Escape key or a key sequence split
					// across reads; only more input or the caller's flush timeout can decide
					mp_InputBuffer = mp_InputBuffer.f_Extract(pParse - pStartParse);

					return;
				}

				switch (*pSequence)
				{
					case '[': // CSI
					{
						++pSequence;
						if (!fp_ParseCsi(pSequence, pEnd))
						{
							// Incomplete: keep from ESC and wait for more input
							mp_InputBuffer = mp_InputBuffer.f_Extract(pParse - pStartParse);

							return;
						}

						pParse = pSequence;

						break;
					}

					case 'O': // SS3
					{
						++pSequence;
						if (pSequence >= pEnd)
						{
							mp_InputBuffer = mp_InputBuffer.f_Extract(pParse - pStartParse);

							return;
						}

						ch32 ScanCode = fg_LetterKeyCode(*pSequence);
						++pSequence;

						if (ScanCode)
						{
							CKeyEvent Event;
							Event.m_ScanCode = ScanCode;
							fp_EmitKey(fg_Move(Event));
						}

						pParse = pSequence;

						break;
					}

					case ']': // OSC
					case 'P': // DCS
					case 'X': // SOS
					case '^': // PM
					case '_': // APC
					{
						// String sequences terminate with ST (ESC \) or BEL; skip their payload
						++pSequence;

						bool bTerminated = false;
						while (pSequence < pEnd)
						{
							if (*pSequence == '\x07')
							{
								++pSequence;
								bTerminated = true;

								break;
							}

							if (*pSequence == '\x1B' && pSequence + 1 < pEnd && *(pSequence + 1) == '\\')
							{
								pSequence += 2;
								bTerminated = true;

								break;
							}

							++pSequence;
						}

						if (!bTerminated)
						{
							// Incomplete: keep from ESC so the remaining payload is not parsed as keystrokes
							mp_InputBuffer = mp_InputBuffer.f_Extract(pParse - pStartParse);

							return;
						}

						pParse = pSequence;

						break;
					}

					case 'N': // SS2
					case '\\': // ST
					{
						++pSequence;
						pParse = pSequence;

						break;
					}

					case '\x1B':
					{
						// ESC ESC: emit Escape, reparse the second ESC
						CKeyEvent Event;
						Event.m_ScanCode = ch32(EKey::mc_Escape);
						fp_EmitKey(fg_Move(Event));

						pParse = pSequence;

						break;
					}

					default:
					{
						// ESC prefixed printable: legacy Alt+key, which may be a multibyte UTF-8 codepoint
						umint SequenceLen = 1;
						uch8 Lead = uch8(*pSequence);
						if ((Lead & uch8(0xE0)) == uch8(0xC0))
							SequenceLen = 2;
						else if ((Lead & uch8(0xF0)) == uch8(0xE0))
							SequenceLen = 3;
						else if ((Lead & uch8(0xF8)) == uch8(0xF0))
							SequenceLen = 4;

						if (umint(pEnd - pSequence) < SequenceLen)
						{
							// Incomplete: keep from ESC and wait for more input
							mp_InputBuffer = mp_InputBuffer.f_Extract(pParse - pStartParse);

							return;
						}

						ch32 Character = ch32(Lead);
						if (SequenceLen > 1)
						{
							if (auto iSequence = fg_GetUnicodeIterator(pSequence, SequenceLen))
								Character = *iSequence;
						}

						CKeyEvent Event;
						Event.m_ScanCode = Character;
						Event.m_Modifiers = EKeyModifier::mc_Alt;
						Event.m_Text = fg_CharToString(Character);
						fp_EmitKey(fg_Move(Event));

						pSequence += SequenceLen;
						pParse = pSequence;

						break;
					}
				}

				pFinishedParse = pParse;

				continue;
			}

			if (uch8(Char) < 0x20 || Char == 0x7F)
			{
				fp_EmitControl(Char);

				++pParse;
				pFinishedParse = pParse;

				continue;
			}

			// A run of printable input: emit one key event per codepoint
			ch8 const *pRunStart = pParse;
			while (pParse < pEnd && uch8(*pParse) >= 0x20 && *pParse != 0x7F)
				++pParse;

			bool bIncompleteCodepoint = false;
			if (pParse == pEnd)
			{
				// Keep an incomplete trailing UTF-8 codepoint buffered until its continuation bytes arrive
				ch8 const *pLead = pParse - 1;
				while (pLead > pRunStart && (uch8(*pLead) & uch8(0xC0)) == uch8(0x80) && pParse - pLead < 4)
					--pLead;

				umint ExpectedLen = 0;
				uch8 Lead = uch8(*pLead);
				if ((Lead & uch8(0xE0)) == uch8(0xC0))
					ExpectedLen = 2;
				else if ((Lead & uch8(0xF0)) == uch8(0xE0))
					ExpectedLen = 3;
				else if ((Lead & uch8(0xF8)) == uch8(0xF0))
					ExpectedLen = 4;

				if (ExpectedLen > umint(pParse - pLead))
				{
					pParse = pLead;
					bIncompleteCodepoint = true;
				}
			}

			for (auto iRun = fg_GetUnicodeIterator(pRunStart, umint(pParse - pRunStart)); iRun; ++iRun)
			{
				ch32 Codepoint = *iRun;
				CKeyEvent Event;
				Event.m_ScanCode = Codepoint;
				Event.m_Text = fg_CharToString(Codepoint);
				fp_EmitKey(fg_Move(Event));
			}

			pFinishedParse = pParse;

			if (bIncompleteCodepoint)
				break;
		}

		mp_InputBuffer = mp_InputBuffer.f_Extract(pFinishedParse - pStartParse);
	}

	bool CAnsiEncodingParseInput::f_HasBufferedInput() const
	{
		return !mp_InputBuffer.f_IsEmpty();
	}

	void CAnsiEncodingParseInput::f_Flush()
	{
		if (mp_InputBuffer.f_IsEmpty())
			return;

		// An incomplete UTF-8 codepoint can only be completed by more input
		if (*mp_InputBuffer.f_GetStr() != '\x1B')
			return;

		// No continuation arrived for the buffered escape: it is the Escape key after all
		CKeyEvent Event;
		Event.m_ScanCode = ch32(EKey::mc_Escape);
		fp_EmitKey(fg_Move(Event));

		CStr Remaining = mp_InputBuffer.f_Extract(1);
		mp_InputBuffer.f_Clear();
		f_AddInput(Remaining);
	}
}
