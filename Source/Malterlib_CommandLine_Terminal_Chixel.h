// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

#define DCommandLineSupportOverflow 1

namespace NMib::NCommandLine
{
	// https://github.com/mintty/mintty/wiki/Tips

	enum class EChixelFlag : uint32
	{
		mc_None              = 0
		, mc_Italic          = DMibBit(0)
		, mc_Strikeout       = DMibBit(1)
		, mc_WeightOffset    = 2
		, mc_WeightSize      = 2
		, mc_WeightMask      = DMibBitRange(mc_WeightOffset, mc_WeightOffset + mc_WeightSize - 1)
		, mc_UnderlineOffset = 4
		, mc_UnderlineSize   = 3
		, mc_UnderlineMask   = DMibBitRange(mc_UnderlineOffset, mc_UnderlineOffset + mc_UnderlineSize - 1)
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
		auto operator <=> (CChixel const &_Right) const = default;

		template <typename tf_CStr>
		void f_Format(tf_CStr &o_Str) const;

		EChixelFlag f_GetFlags() const;
		ch32 f_GetCharacter() const;
		ETerminalEndpointType f_GetEndpointType();
		ch32 f_GetCharacterAndEndpointType();
		void f_SetFlags(EChixelFlag _Flags);
		void f_AddFlags(EChixelFlag _Flags);
		void f_ChangeFlags(EChixelFlag _Remove, EChixelFlag _Add);
		void f_RemoveFlags(EChixelFlag _Flags);
		void f_SetEndpointType(ETerminalEndpointType _Type);
		void f_ClearOverflow();
		void f_SetCharacter(ch32 _Character);
		void f_SetAll(ch32 _Character, EChixelFlag _Flags, ETerminalEndpointType _Type = ETerminalEndpointType::mc_Unspecified);
		void f_SetCharacterAndEndpointType(ch32 _Character, ETerminalEndpointType _Type = ETerminalEndpointType::mc_Unspecified);
		void f_SetCharacterAndEndpointTypeRaw(ch32 _CharacterAndEndpointType);

		CAnsiEncoding::EWeight f_Weight() const;
		CAnsiEncoding::EUnderline f_Underline() const;
		bool f_Italic() const;
		bool f_Strikeout() const;

		void f_Weight(CAnsiEncoding::EWeight _Value);
		void f_Underline(CAnsiEncoding::EUnderline _Value);
		void f_Italic(bool _bValue);
		void f_Strikeout(bool _bValue);

		static constexpr umint mc_CharacterOffset = 0;
		static constexpr umint mc_CharacterBits = 21;
		static constexpr ch32 mc_CharacterMask = DMibBitRangeTyped(mc_CharacterOffset, mc_CharacterOffset + mc_CharacterBits - 1, ch32);

		static constexpr umint mc_FlagsOffset = mc_CharacterOffset + mc_CharacterBits;
		static constexpr umint mc_FlagsBits = 7;
		static constexpr ch32 mc_FlagsMask = DMibBitRangeTyped(mc_FlagsOffset, mc_FlagsOffset + mc_FlagsBits - 1, ch32);

		static constexpr umint mc_EndpointOffset = mc_FlagsOffset + mc_FlagsBits;
		static constexpr umint mc_EndpointBits = 4;
		static constexpr ch32 mc_EndpointMask = DMibBitRangeTyped(mc_EndpointOffset, mc_EndpointOffset + mc_EndpointBits - 1, ch32);

		ch32 m_CharaterFlagsAndEndpointType = 0; // One Unicode codepoint; combining codepoints live in the owning surface.
		CTerminalColor m_BackgroundColor;
		CTerminalColor m_ForegroundColor;
		CTerminalColor m_UnderlineColor;
#if DCommandLineSupportOverflow
		bool m_bHasOverflow = false; // The owning surface stores additional codepoints by cell index.
#endif
	};

	constexpr ch32 fg_CharacterAndEndpointType(ch32 _Character, ETerminalEndpointType _Type);

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
