// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

{
	.m_Combined = str_utf32('─')
	, .m_Left = gc_Str<str_utf8("╌┄┈╴╶")>
	, .m_Right = gc_Str<str_utf8("─")>
	, .m_EndpointTypesLeft = {DLightOnward}
	DInsertFileLine
}
,
{
	.m_Combined = str_utf32('─')
	, .m_Left = gc_Str<str_utf8("╌┄┈╶")>
	, .m_Right = gc_Str<str_utf8("╴")>
	, .m_EndpointTypesLeft = {DLightOnward}
	, .m_EndpointTypesRight = {ETerminalEndpointType::mc_Light, ETerminalEndpointType::mc_LightRounded}
	DInsertFileLine
}
,
{
	.m_Combined = str_utf32('─')
	, .m_Left = gc_Str<str_utf8("╌┄┈╴")>
	, .m_Right = gc_Str<str_utf8("╶")>
	, .m_EndpointTypesLeft = {DLightOnward}
	, .m_EndpointTypesRight = {ETerminalEndpointType::mc_Light, ETerminalEndpointType::mc_LightRounded}
	DInsertFileLine
}
,
// ╌
{
	.m_Combined = str_utf32('╌')
	, .m_Left = gc_Str<str_utf8("┄┈╴╶")>
	, .m_Right = gc_Str<str_utf8("╌")>
	, .m_EndpointTypesLeft = {DLightAllDotted3Onward}
	DInsertFileLine
}
,
{
	.m_Combined = str_utf32('╌')
	, .m_Left = gc_Str<str_utf8("┄┈╶")>
	, .m_Right = gc_Str<str_utf8("╴")>
	, .m_EndpointTypesLeft = {DLightAllDotted3Onward}
	, .m_EndpointTypesRight = {ETerminalEndpointType::mc_LightDotted3, ETerminalEndpointType::mc_LightDotted3Rounded}
	DInsertFileLine
}
,
{
	.m_Combined = str_utf32('╌')
	, .m_Left = gc_Str<str_utf8("┄┈╴")>
	, .m_Right = gc_Str<str_utf8("╶")>
	, .m_EndpointTypesLeft = {DLightAllDotted3Onward}
	, .m_EndpointTypesRight = {ETerminalEndpointType::mc_LightDotted3, ETerminalEndpointType::mc_LightDotted3Rounded}
	DInsertFileLine
}
,
// ┄
{
	.m_Combined = str_utf32('┄')
	, .m_Left = gc_Str<str_utf8("┈╴╶")>
	, .m_Right = gc_Str<str_utf8("┄")>
	, .m_EndpointTypesLeft = {DLightAllDotted2Onward}
	DInsertFileLine
}
,
{
	.m_Combined = str_utf32('┄')
	, .m_Left = gc_Str<str_utf8("┈╶")>
	, .m_Right = gc_Str<str_utf8("╴")>
	, .m_EndpointTypesLeft = {DLightAllDotted2Onward}
	, .m_EndpointTypesRight = {ETerminalEndpointType::mc_LightDotted2, ETerminalEndpointType::mc_LightDotted2Rounded}
	DInsertFileLine
}
,
{
	.m_Combined = str_utf32('┄')
	, .m_Left = gc_Str<str_utf8("┈╴")>
	, .m_Right = gc_Str<str_utf8("╶")>
	, .m_EndpointTypesLeft = {DLightAllDotted2Onward}
	, .m_EndpointTypesRight = {ETerminalEndpointType::mc_LightDotted2, ETerminalEndpointType::mc_LightDotted2Rounded}
	DInsertFileLine
}
,
// ┈
{
	.m_Combined = str_utf32('┈')
	, .m_Left = gc_Str<str_utf8("╴╶")>
	, .m_Right = gc_Str<str_utf8("┈")>
	, .m_EndpointTypesLeft = {DLightAllDotted1Onward}
	DInsertFileLine
}
,
{
	.m_Combined = str_utf32('┈')
	, .m_Left = gc_Str<str_utf8("╶")>
	, .m_Right = gc_Str<str_utf8("╴")>
	, .m_EndpointTypesLeft = {DLightAllDotted1Onward}
	, .m_EndpointTypesRight = {ETerminalEndpointType::mc_LightDotted1, ETerminalEndpointType::mc_LightDotted1Rounded}
	DInsertFileLine
}
,
