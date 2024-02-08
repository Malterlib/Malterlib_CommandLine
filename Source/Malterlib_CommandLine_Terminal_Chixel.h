// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

#define DCommandLineSupportOverflow 1

namespace NMib::NCommandLine
{
	// https://github.com/mintty/mintty/wiki/Tips

	enum class EChixelFlag : uint32
	{
		mc_None = 0
		, mc_Italic = DMibBit(0)
		, mc_Strikeout = DMibBit(1)
		, mc_WeightOffset = 2
		, mc_WeightSize = 2
		, mc_WeightMask = DMibBitRange(mc_WeightOffset, mc_WeightOffset + mc_WeightSize - 1)
		, mc_UnderlineOffset = 4
		, mc_UnderlineSize = 3
		, mc_UnderlineMask = DMibBitRange(mc_UnderlineOffset, mc_UnderlineOffset + mc_UnderlineSize - 1)
	};

	enum class ETerminalEndpointType : uint8
	{
		mc_Unspecified = 0
		, mc_Heavy
		, mc_HeavyDotted3
		, mc_HeavyDotted2
		, mc_HeavyDotted1
		, mc_Double
		, mc_Light
		, mc_LightDotted3
		, mc_LightDotted2
		, mc_LightDotted1
		, mc_LightRounded
		, mc_LightDotted3Rounded
		, mc_LightDotted2Rounded
		, mc_LightDotted1Rounded
	};

	struct CChixelProperties
	{
		void f_ChangeFlags(EChixelFlag _Remove, EChixelFlag _Add);

		template <typename tf_CStr>
		void f_Format(tf_CStr &o_Str) const;

		CAnsiEncoding::EWeight f_Weight() const;
		CAnsiEncoding::EUnderline f_Underline() const;
		bool f_Italic() const;
		bool f_Strikeout() const;

		void f_Weight(CAnsiEncoding::EWeight _Value);
		void f_Underline(CAnsiEncoding::EUnderline _Value);
		void f_Italic(bool _bValue);
		void f_Strikeout(bool _bValue);

		static CAnsiEncoding::EWeight fs_Weight(EChixelFlag _Flags);
		static CAnsiEncoding::EUnderline fs_Underline(EChixelFlag _Flags);

		static EChixelFlag fs_WeightToFlags(CAnsiEncoding::EWeight _Weight);
		static EChixelFlag fs_UnderlineToFlags(CAnsiEncoding::EUnderline _Underline);

		static NStr::CStr fs_WeightToString(CAnsiEncoding::EWeight _Weight);
		static NStr::CStr fs_UnderlineToString(CAnsiEncoding::EUnderline _Underline);

		EChixelFlag m_Flags = EChixelFlag::mc_None;
		CTerminalColor m_BackgroundColor;
		CTerminalColor m_ForegroundColor;
		CTerminalColor m_UnderlineColor;
	};

	struct CChixel
	{
		auto operator <=> (CChixel const &) const = default;

		template <typename tf_CStr>
		void f_Format(tf_CStr &o_Str) const;

		EChixelFlag f_GetFlags() const
		{
			return EChixelFlag((m_CharaterFlagsAndEndpointType & mc_FlagsMask) >> mc_FlagsOffset);
		}

		ch32 f_GetCharacter() const
		{
			return m_CharaterFlagsAndEndpointType & mc_CharacterMask;
		}

		ETerminalEndpointType f_GetEndpointType()
		{
			return ETerminalEndpointType((m_CharaterFlagsAndEndpointType & mc_EndpointMask) >> mc_EndpointOffset);
		}

		ch32 f_GetCharacterAndEndpointType()
		{
			return (m_CharaterFlagsAndEndpointType & (mc_CharacterMask | mc_EndpointMask));
		}

		void f_SetFlags(EChixelFlag _Flags)
		{
			m_CharaterFlagsAndEndpointType = (m_CharaterFlagsAndEndpointType & ~mc_FlagsMask) | ch32(_Flags) << mc_FlagsOffset;
		}

		void f_AddFlags(EChixelFlag _Flags)
		{
			m_CharaterFlagsAndEndpointType |= (ch32(_Flags) << mc_FlagsOffset);
		}

		void f_ChangeFlags(EChixelFlag _Remove, EChixelFlag _Add)
		{
			m_CharaterFlagsAndEndpointType = (m_CharaterFlagsAndEndpointType & ~(ch32(_Remove) << mc_FlagsOffset)) | (ch32(_Add) << mc_FlagsOffset);
		}

		void f_RemoveFlags(EChixelFlag _Flags)
		{
			m_CharaterFlagsAndEndpointType &= ~(ch32(_Flags) << mc_FlagsOffset);
		}

		void f_SetEndpointType(ETerminalEndpointType _Type)
		{
			m_CharaterFlagsAndEndpointType = (m_CharaterFlagsAndEndpointType & ~mc_EndpointMask) | ch32(_Type) << mc_EndpointOffset;
		}

		void f_ClearOverflow()
		{
#if DCommandLineSupportOverflow
			m_bHasOverflow = false;
#endif
		}

		void f_SetCharacter(ch32 _Character)
		{
			f_ClearOverflow();
			m_CharaterFlagsAndEndpointType = (m_CharaterFlagsAndEndpointType & ~mc_CharacterMask) | _Character;
		}

		void f_SetAll(ch32 _Character, EChixelFlag _Flags, ETerminalEndpointType _Type = ETerminalEndpointType::mc_Unspecified)
		{
			f_ClearOverflow();
			m_CharaterFlagsAndEndpointType = _Character | (ch32(_Flags) << mc_FlagsOffset) | (ch32(_Type) << mc_EndpointOffset);
		}

		void f_SetCharacterAndEndpointType(ch32 _Character, ETerminalEndpointType _Type = ETerminalEndpointType::mc_Unspecified)
		{
			f_ClearOverflow();
			m_CharaterFlagsAndEndpointType = (m_CharaterFlagsAndEndpointType & ~(mc_EndpointMask | mc_CharacterMask)) | _Character | ch32(_Type) << mc_EndpointOffset;
		}

		void f_SetCharacterAndEndpointTypeRaw(ch32 _CharacterAndEndpointType)
		{
			f_ClearOverflow();
			m_CharaterFlagsAndEndpointType = (m_CharaterFlagsAndEndpointType & ~(mc_EndpointMask | mc_CharacterMask)) | _CharacterAndEndpointType;
		}

		CAnsiEncoding::EWeight f_Weight() const;
		CAnsiEncoding::EUnderline f_Underline() const;
		bool f_Italic() const;
		bool f_Strikeout() const;

		void f_Weight(CAnsiEncoding::EWeight _Value);
		void f_Underline(CAnsiEncoding::EUnderline _Value);
		void f_Italic(bool _bValue);
		void f_Strikeout(bool _bValue);

		constexpr static umint mc_CharacterOffset = 0;
		constexpr static umint mc_CharacterBits = 21;
		constexpr static ch32 mc_CharacterMask = DMibBitRangeTyped(mc_CharacterOffset, mc_CharacterOffset + mc_CharacterBits - 1, ch32);

		constexpr static umint mc_FlagsOffset = mc_CharacterOffset + mc_CharacterBits;
		constexpr static umint mc_FlagsBits = 7;
		constexpr static ch32 mc_FlagsMask = DMibBitRangeTyped(mc_FlagsOffset, mc_FlagsOffset + mc_FlagsBits - 1, ch32);

		constexpr static umint mc_EndpointOffset = mc_FlagsOffset + mc_FlagsBits;
		constexpr static umint mc_EndpointBits = 4;
		constexpr static ch32 mc_EndpointMask = DMibBitRangeTyped(mc_EndpointOffset, mc_EndpointOffset + mc_EndpointBits - 1, ch32);

		ch32 m_CharaterFlagsAndEndpointType; // Normally one displayed charater fits in one unicode codepoint
		CTerminalColor m_BackgroundColor;
		CTerminalColor m_ForegroundColor;
		CTerminalColor m_UnderlineColor;
#if DCommandLineSupportOverflow
		// Characters that do not fit in one codepoint (combining characters) keep the extra
		// codepoints in the owning surface, keyed by cell index; the chixel itself stays
		// trivially copyable so the bulk per-cell loops are plain scalar copies and compares
		bool m_bHasOverflow = false;
#endif

		// Total size: 20 bytes
		// MBP buffer size = 238 x  89 x 20 =  423640 bytes at 60 Hz = 24 MiB/second
		// 5K buffer size  = 425 x 143 x 20 = 1215500 bytes at 60 Hz = 70 MiB/second
	};

	constexpr ch32 fg_CharacterAndEndpointType(ch32 _Character, ETerminalEndpointType _Type)
	{
		return _Character | (ch32(_Type) << CChixel::mc_EndpointOffset);
	}

	struct CSetChixelProperties
	{
		void f_ChangeFlags(EChixelFlag _Remove, EChixelFlag _Add);

		CAnsiEncoding::EWeight f_Weight() const;
		CAnsiEncoding::EUnderline f_Underline() const;
		bool f_Italic() const;
		bool f_Strikeout() const;

		void f_Weight(CAnsiEncoding::EWeight _Value);
		void f_Underline(CAnsiEncoding::EUnderline _Value);
		void f_Italic(bool _bValue);
		void f_Strikeout(bool _bValue);

		template <typename tf_CStr>
		void f_Format(tf_CStr &o_Str) const;

		NStorage::TCOptional<CTerminalColor> m_BackgroundColor;
		NStorage::TCOptional<CTerminalColor> m_ForegroundColor;
		NStorage::TCOptional<CTerminalColor> m_UnderlineColor;

		NStorage::TCOptional<NGraphics::CColorBlend> m_BackgroundBlend;
		NStorage::TCOptional<NGraphics::CColorBlend> m_ForegroundBlend;
		NStorage::TCOptional<NGraphics::CColorBlend> m_UnderlineBlend;

		NStorage::TCOptional<EChixelFlag> m_ChixelFlags;
	};
}

#include "Malterlib_CommandLine_Terminal_Chixel.hpp"
