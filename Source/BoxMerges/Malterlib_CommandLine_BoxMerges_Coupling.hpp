// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

{
	.m_Combined = str_utf32('╽')
	, .m_Left = gc_Str<str_utf8("│╎┆┊╻╵╷")>
	, .m_Right = gc_Str<str_utf8("╽")>
	, .m_EndpointTypesLeft = {DHeavyOnward}
	DInsertFileLine
}
,
{
	.m_Combined = str_utf32('╽')
	, .m_Left = gc_Str<str_utf8("│╵")>
	, .m_Right = gc_Str<str_utf8("╻")>
	, .m_EndpointTypesLeft = {ETerminalEndpointType::mc_Light, ETerminalEndpointType::mc_LightRounded}
	, .m_EndpointTypesRight = {ETerminalEndpointType::mc_Heavy}
	DInsertFileLine
}
,
// ╿
{
	.m_Combined = str_utf32('╿')
	, .m_Left = gc_Str<str_utf8("│╎┆┊╹╵╷")>
	, .m_Right = gc_Str<str_utf8("╿")>
	, .m_EndpointTypesLeft = {DHeavyOnward}
	DInsertFileLine
}
,
{
	.m_Combined = str_utf32('╿')
	, .m_Left = gc_Str<str_utf8("╹")>
	, .m_Right = gc_Str<str_utf8("│╷")>
	, .m_EndpointTypesLeft = {ETerminalEndpointType::mc_Heavy}
	, .m_EndpointTypesRight = {ETerminalEndpointType::mc_Light, ETerminalEndpointType::mc_LightRounded}
	DInsertFileLine
}
,
// ╼
{
	.m_Combined = str_utf32('╼')
	, .m_Left = gc_Str<str_utf8("─╌┄┈╺╴╶")>
	, .m_Right = gc_Str<str_utf8("╼")>
	, .m_EndpointTypesLeft = {DHeavyOnward}
	DInsertFileLine
}
,
{
	.m_Combined = str_utf32('╼')
	, .m_Left = gc_Str<str_utf8("╺")>
	, .m_Right = gc_Str<str_utf8("─╴")>
	, .m_EndpointTypesLeft = {ETerminalEndpointType::mc_Heavy}
	, .m_EndpointTypesRight = {ETerminalEndpointType::mc_Light, ETerminalEndpointType::mc_LightRounded}
	DInsertFileLine
}
,
// ╾
{
	.m_Combined = str_utf32('╾')
	, .m_Left = gc_Str<str_utf8("─╌┄┈╸╴╶")>
	, .m_Right = gc_Str<str_utf8("╾")>
	, .m_EndpointTypesLeft = {DHeavyOnward}
	DInsertFileLine
}
,
{
	.m_Combined = str_utf32('╾')
	, .m_Left = gc_Str<str_utf8("╸")>
	, .m_Right = gc_Str<str_utf8("─╶")>
	, .m_EndpointTypesLeft = {ETerminalEndpointType::mc_Heavy}
	, .m_EndpointTypesRight = {ETerminalEndpointType::mc_Light, ETerminalEndpointType::mc_LightRounded}
	DInsertFileLine
}
,
