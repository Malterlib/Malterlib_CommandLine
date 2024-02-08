// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

{
	.m_Combined = fg_CharacterAndEndpointType(str_utf32('╺'), ETerminalEndpointType::mc_Heavy)
	, .m_Left = gc_Str<str_utf8("╺╶")>
	, .m_Right = gc_Str<str_utf8("╺")>
	, .m_EndpointTypesLeft =
		{
			ETerminalEndpointType::mc_Heavy
			, ETerminalEndpointType::mc_HeavyDotted3
			, ETerminalEndpointType::mc_HeavyDotted2
			, ETerminalEndpointType::mc_HeavyDotted1
			, ETerminalEndpointType::mc_Double
			, ETerminalEndpointType::mc_Light
			, ETerminalEndpointType::mc_LightDotted3
			, ETerminalEndpointType::mc_LightDotted2
			, ETerminalEndpointType::mc_LightDotted1
			, ETerminalEndpointType::mc_LightRounded
			, ETerminalEndpointType::mc_LightDotted3Rounded
			, ETerminalEndpointType::mc_LightDotted2Rounded
			, ETerminalEndpointType::mc_LightDotted1Rounded
		}
	, .m_EndpointTypesRight = {ETerminalEndpointType::mc_Heavy}
	DInsertFileLine
}
,
{
	.m_Combined = fg_CharacterAndEndpointType(str_utf32('╺'), ETerminalEndpointType::mc_HeavyDotted3)
	, .m_Left = gc_Str<str_utf8("╺╶")>
	, .m_Right = gc_Str<str_utf8("╺")>
	, .m_EndpointTypesLeft =
		{
			ETerminalEndpointType::mc_HeavyDotted3
			, ETerminalEndpointType::mc_HeavyDotted2
			, ETerminalEndpointType::mc_HeavyDotted1
			, ETerminalEndpointType::mc_Double
			, ETerminalEndpointType::mc_Light
			, ETerminalEndpointType::mc_LightDotted3
			, ETerminalEndpointType::mc_LightDotted2
			, ETerminalEndpointType::mc_LightDotted1
			, ETerminalEndpointType::mc_LightRounded
			, ETerminalEndpointType::mc_LightDotted3Rounded
			, ETerminalEndpointType::mc_LightDotted2Rounded
			, ETerminalEndpointType::mc_LightDotted1Rounded
		}
	, .m_EndpointTypesRight = {ETerminalEndpointType::mc_HeavyDotted3}
	DInsertFileLine
}
,
{
	.m_Combined = fg_CharacterAndEndpointType(str_utf32('╺'), ETerminalEndpointType::mc_HeavyDotted2)
	, .m_Left = gc_Str<str_utf8("╺╶")>
	, .m_Right = gc_Str<str_utf8("╺")>
	, .m_EndpointTypesLeft =
		{
			ETerminalEndpointType::mc_HeavyDotted2
			, ETerminalEndpointType::mc_HeavyDotted1
			, ETerminalEndpointType::mc_Double
			, ETerminalEndpointType::mc_Light
			, ETerminalEndpointType::mc_LightDotted3
			, ETerminalEndpointType::mc_LightDotted2
			, ETerminalEndpointType::mc_LightDotted1
			, ETerminalEndpointType::mc_LightRounded
			, ETerminalEndpointType::mc_LightDotted3Rounded
			, ETerminalEndpointType::mc_LightDotted2Rounded
			, ETerminalEndpointType::mc_LightDotted1Rounded
		}
	, .m_EndpointTypesRight = {ETerminalEndpointType::mc_HeavyDotted2}
	DInsertFileLine
}
,
{
	.m_Combined = fg_CharacterAndEndpointType(str_utf32('╺'), ETerminalEndpointType::mc_HeavyDotted1)
	, .m_Left = gc_Str<str_utf8("╺╶")>
	, .m_Right = gc_Str<str_utf8("╺")>
	, .m_EndpointTypesLeft =
		{
			ETerminalEndpointType::mc_HeavyDotted1
			, ETerminalEndpointType::mc_Double
			, ETerminalEndpointType::mc_Light
			, ETerminalEndpointType::mc_LightDotted3
			, ETerminalEndpointType::mc_LightDotted2
			, ETerminalEndpointType::mc_LightDotted1
			, ETerminalEndpointType::mc_LightRounded
			, ETerminalEndpointType::mc_LightDotted3Rounded
			, ETerminalEndpointType::mc_LightDotted2Rounded
			, ETerminalEndpointType::mc_LightDotted1Rounded
		}
	, .m_EndpointTypesRight = {ETerminalEndpointType::mc_HeavyDotted1}
	DInsertFileLine
}
,
{
	.m_Combined = fg_CharacterAndEndpointType(str_utf32('╺'), ETerminalEndpointType::mc_Double)
	, .m_Left = gc_Str<str_utf8("╺╶")>
	, .m_Right = gc_Str<str_utf8("╺")>
	, .m_EndpointTypesLeft =
		{
			ETerminalEndpointType::mc_Double
			, ETerminalEndpointType::mc_Light
			, ETerminalEndpointType::mc_LightDotted3
			, ETerminalEndpointType::mc_LightDotted2
			, ETerminalEndpointType::mc_LightDotted1
			, ETerminalEndpointType::mc_LightRounded
			, ETerminalEndpointType::mc_LightDotted3Rounded
			, ETerminalEndpointType::mc_LightDotted2Rounded
			, ETerminalEndpointType::mc_LightDotted1Rounded
		}
	, .m_EndpointTypesRight = {ETerminalEndpointType::mc_Double}
	DInsertFileLine
}
,
// ╶
{
	.m_Combined = fg_CharacterAndEndpointType(str_utf32('╶'), ETerminalEndpointType::mc_Light)
	, .m_Left = gc_Str<str_utf8("╶")>
	, .m_Right = gc_Str<str_utf8("╶")>
	, .m_EndpointTypesLeft =
		{
			ETerminalEndpointType::mc_Light
			, ETerminalEndpointType::mc_LightDotted3
			, ETerminalEndpointType::mc_LightDotted2
			, ETerminalEndpointType::mc_LightDotted1
			, ETerminalEndpointType::mc_LightRounded
			, ETerminalEndpointType::mc_LightDotted3Rounded
			, ETerminalEndpointType::mc_LightDotted2Rounded
			, ETerminalEndpointType::mc_LightDotted1Rounded
		}
	, .m_EndpointTypesRight = {ETerminalEndpointType::mc_Light}
	DInsertFileLine
}
,
{
	.m_Combined = fg_CharacterAndEndpointType(str_utf32('╶'), ETerminalEndpointType::mc_LightDotted3)
	, .m_Left = gc_Str<str_utf8("╶")>
	, .m_Right = gc_Str<str_utf8("╶")>
	, .m_EndpointTypesLeft =
		{
			ETerminalEndpointType::mc_LightDotted3
			, ETerminalEndpointType::mc_LightDotted2
			, ETerminalEndpointType::mc_LightDotted1
			, ETerminalEndpointType::mc_LightRounded
			, ETerminalEndpointType::mc_LightDotted3Rounded
			, ETerminalEndpointType::mc_LightDotted2Rounded
			, ETerminalEndpointType::mc_LightDotted1Rounded
		}
	, .m_EndpointTypesRight = {ETerminalEndpointType::mc_LightDotted3}
	DInsertFileLine
}
,
{
	.m_Combined = fg_CharacterAndEndpointType(str_utf32('╶'), ETerminalEndpointType::mc_LightDotted2)
	, .m_Left = gc_Str<str_utf8("╶")>
	, .m_Right = gc_Str<str_utf8("╶")>
	, .m_EndpointTypesLeft =
		{
			ETerminalEndpointType::mc_LightDotted2
			, ETerminalEndpointType::mc_LightDotted1
			, ETerminalEndpointType::mc_LightRounded
			, ETerminalEndpointType::mc_LightDotted3Rounded
			, ETerminalEndpointType::mc_LightDotted2Rounded
			, ETerminalEndpointType::mc_LightDotted1Rounded
		}
	, .m_EndpointTypesRight = {ETerminalEndpointType::mc_LightDotted2}
	DInsertFileLine
}
,
{
	.m_Combined = fg_CharacterAndEndpointType(str_utf32('╶'), ETerminalEndpointType::mc_LightDotted1)
	, .m_Left = gc_Str<str_utf8("╶")>
	, .m_Right = gc_Str<str_utf8("╶")>
	, .m_EndpointTypesLeft =
		{
			ETerminalEndpointType::mc_LightDotted1
			, ETerminalEndpointType::mc_LightRounded
			, ETerminalEndpointType::mc_LightDotted3Rounded
			, ETerminalEndpointType::mc_LightDotted2Rounded
			, ETerminalEndpointType::mc_LightDotted1Rounded
		}
	, .m_EndpointTypesRight = {ETerminalEndpointType::mc_LightDotted1}
	DInsertFileLine
}
,
{
	.m_Combined = fg_CharacterAndEndpointType(str_utf32('╶'), ETerminalEndpointType::mc_LightRounded)
	, .m_Left = gc_Str<str_utf8("╶")>
	, .m_Right = gc_Str<str_utf8("╶")>
	, .m_EndpointTypesLeft =
		{
			ETerminalEndpointType::mc_LightRounded
			, ETerminalEndpointType::mc_LightDotted3Rounded
			, ETerminalEndpointType::mc_LightDotted2Rounded
			, ETerminalEndpointType::mc_LightDotted1Rounded
		}
	, .m_EndpointTypesRight = {ETerminalEndpointType::mc_LightRounded}
	DInsertFileLine
}
,
{
	.m_Combined = fg_CharacterAndEndpointType(str_utf32('╶'), ETerminalEndpointType::mc_LightDotted3Rounded)
	, .m_Left = gc_Str<str_utf8("╶")>
	, .m_Right = gc_Str<str_utf8("╶")>
	, .m_EndpointTypesLeft = {ETerminalEndpointType::mc_LightDotted3Rounded, ETerminalEndpointType::mc_LightDotted2Rounded, ETerminalEndpointType::mc_LightDotted1Rounded}
	, .m_EndpointTypesRight = {ETerminalEndpointType::mc_LightDotted3Rounded}
	DInsertFileLine
}
,
{
	.m_Combined = fg_CharacterAndEndpointType(str_utf32('╶'), ETerminalEndpointType::mc_LightDotted2Rounded)
	, .m_Left = gc_Str<str_utf8("╶")>
	, .m_Right = gc_Str<str_utf8("╶")>
	, .m_EndpointTypesLeft = {ETerminalEndpointType::mc_LightDotted2Rounded, ETerminalEndpointType::mc_LightDotted1Rounded}
	, .m_EndpointTypesRight = {ETerminalEndpointType::mc_LightDotted2Rounded}
	DInsertFileLine
}
,
