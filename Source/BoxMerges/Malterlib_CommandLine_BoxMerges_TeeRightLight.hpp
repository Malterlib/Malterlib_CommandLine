// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

{
	.m_Combined = str_utf32('├')
	, .m_Left = gc_Str<str_utf8("│╎┆┊└╰┌╭╵╶╷")>
	, .m_Right = gc_Str<str_utf8("├")>
	, .m_EndpointTypesLeft = {DLightOnward}
	DInsertFileLine
}
,
{
	.m_Combined = str_utf32('├')
	, .m_Left = gc_Str<str_utf8("└╰┌╭╶")>
	, .m_Right = gc_Str<str_utf8("│╎┆┊")>
	, .m_EndpointTypesLeft = {DLightOnward}
	DInsertFileLine
}
,
{
	.m_Combined = str_utf32('├')
	, .m_Left = gc_Str<str_utf8("│╎┆┊┌╭╷")>
	, .m_Right = gc_Str<str_utf8("└╰")>
	, .m_EndpointTypesLeft = {DLightOnward}
	DInsertFileLine
}
,
{
	.m_Combined = str_utf32('├')
	, .m_Left = gc_Str<str_utf8("│╎┆┊└╰╵")>
	, .m_Right = gc_Str<str_utf8("┌╭")>
	, .m_EndpointTypesLeft = {DLightOnward}
	DInsertFileLine
}
,
