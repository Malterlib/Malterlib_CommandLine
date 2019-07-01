// Copyright © 2018 Nonna Holding AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

namespace NMib::NCommandLine
{
	enum EAnsiEncodingFlag
	{
		EAnsiEncodingFlag_None = 0
		, EAnsiEncodingFlag_Color = DMibBit(1)
		, EAnsiEncodingFlag_Color24Bit = DMibBit(2)
		, EAnsiEncodingFlag_ColorLightBackground = DMibBit(3)
	};

	struct CAnsiEncoding
	{
		CAnsiEncoding(EAnsiEncodingFlag _Flags);

		NStr::CStr f_ReEncode(NStr::CStr const &_In) const;
		NContainer::TCVector<NStr::CStr> f_LineBreak(NStr::CStr const &_String, mint _Length) const;

		NStr::CStr f_Foreground16(uint8 _Color) const;
		NStr::CStr f_Background16(uint8 _Color) const;
		NStr::CStr f_Foreground256(uint8 _Color) const;
		NStr::CStr f_Background256(uint8 _Color) const;
		NStr::CStr f_ForegroundRGB(uint8 _Red, uint8 _Green, uint8 _Blue) const;
		NStr::CStr f_BackgroundRGB(uint8 _Red, uint8 _Green, uint8 _Blue) const;

		NStr::CStr f_Default() const;
		NStr::CStr f_StatusNormal() const;
		NStr::CStr f_StatusNormal(NStr::CStr const &_ToWrap) const;
		NStr::CStr f_StatusWarning() const;
		NStr::CStr f_StatusWarning(NStr::CStr const &_ToWrap) const;
		NStr::CStr f_StatusError() const;
		NStr::CStr f_StatusError(NStr::CStr const &_ToWrap) const;
		NStr::CStr f_Bold() const;
		NStr::CStr f_Bold(NStr::CStr const &_ToWrap) const;
		NStr::CStr f_Prompt() const;
		NStr::CStr f_Prompt(NStr::CStr const &_ToWrap) const;

		bool f_Color() const;

	private:
		EAnsiEncodingFlag mp_Flags = EAnsiEncodingFlag_None;
	};
}

#ifndef DMibPNoShortCuts
	using namespace NMib::NCommandLine;
#endif
