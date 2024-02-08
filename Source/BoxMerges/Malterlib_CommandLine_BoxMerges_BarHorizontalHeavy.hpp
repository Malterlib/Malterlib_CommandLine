// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

{
	.m_Combined = str_utf32('━')
	, .m_Left = gc_Str<str_utf8("╍┅┉═─╌┄┈╸╺╴╶╼╾")>
	, .m_Right = gc_Str<str_utf8("━")>
	, .m_EndpointTypesLeft = {DHeavyOnward}
	DInsertFileLine
}
,
{
	.m_Combined = str_utf32('━')
	, .m_Left = gc_Str<str_utf8("╍┅┉═╺╼")>
	, .m_Right = gc_Str<str_utf8("╸╾")>
	, .m_EndpointTypesLeft = {DAllHeavyAndDouble}
	, .m_EndpointTypesRight = {ETerminalEndpointType::mc_Heavy}
	DInsertFileLine
}
,
{
	.m_Combined = str_utf32('━')
	, .m_Left = gc_Str<str_utf8("╍┅┉═╸╾")>
	, .m_Right = gc_Str<str_utf8("╺╼")>
	, .m_EndpointTypesLeft = {DAllHeavyAndDouble}
	, .m_EndpointTypesRight = {ETerminalEndpointType::mc_Heavy}
	DInsertFileLine
}
,
{
	.m_Combined = str_utf32('━')
	, .m_Left = gc_Str<str_utf8("╶╌┄┈")>
	, .m_Right = gc_Str<str_utf8("╸")>
	, .m_EndpointTypesLeft = {DLightOnwardNoOpaque}
	, .m_EndpointTypesRight = {ETerminalEndpointType::mc_Heavy}
	DInsertFileLine
}
,
{
	.m_Combined = str_utf32('━')
	, .m_Left = gc_Str<str_utf8("╴╌┄┈")>
	, .m_Right = gc_Str<str_utf8("╺")>
	, .m_EndpointTypesLeft = {DLightOnwardNoOpaque}
	, .m_EndpointTypesRight = {ETerminalEndpointType::mc_Heavy}
	DInsertFileLine
}
,
// ╍
{
	.m_Combined = str_utf32('╍')
	, .m_Left = gc_Str<str_utf8("┅┉═─╌┄┈╸╺╴╶")>
	, .m_Right = gc_Str<str_utf8("╍")>
	, .m_EndpointTypesLeft = {DHeavyDotted3Onward}
	DInsertFileLine
}
,
{
	.m_Combined = str_utf32('╍')
	, .m_Left = gc_Str<str_utf8("╍┅┉═╺")>
	, .m_Right = gc_Str<str_utf8("╸")>
	, .m_EndpointTypesLeft = {DHeavyDotted3Onward}
	, .m_EndpointTypesRight = {ETerminalEndpointType::mc_HeavyDotted3}
	DInsertFileLine
}
,
{
	.m_Combined = str_utf32('╍')
	, .m_Left = gc_Str<str_utf8("╍┅┉═╸")>
	, .m_Right = gc_Str<str_utf8("╺")>
	, .m_EndpointTypesLeft = {DHeavyDotted3Onward}
	, .m_EndpointTypesRight = {ETerminalEndpointType::mc_HeavyDotted3}
	DInsertFileLine
}
,
{
	.m_Combined = str_utf32('╍')
	, .m_Left = gc_Str<str_utf8("╶─╌┄┈")>
	, .m_Right = gc_Str<str_utf8("╸")>
	, .m_EndpointTypesLeft = {DLightOnward}
	, .m_EndpointTypesRight = {ETerminalEndpointType::mc_HeavyDotted3}
	DInsertFileLine
}
,
{
	.m_Combined = str_utf32('╍')
	, .m_Left = gc_Str<str_utf8("╴─╌┄┈")>
	, .m_Right = gc_Str<str_utf8("╺")>
	, .m_EndpointTypesLeft = {DLightOnward}
	, .m_EndpointTypesRight = {ETerminalEndpointType::mc_HeavyDotted3}
	DInsertFileLine
}
,
// ┅
{
	.m_Combined = str_utf32('┅')
	, .m_Left = gc_Str<str_utf8("┉═─╌┄┈╸╺╴╶")>
	, .m_Right = gc_Str<str_utf8("┅")>
	, .m_EndpointTypesLeft = {DHeavyDotted2Onward}
	DInsertFileLine
}
,
{
	.m_Combined = str_utf32('┅')
	, .m_Left = gc_Str<str_utf8("┉═╺")>
	, .m_Right = gc_Str<str_utf8("╸")>
	, .m_EndpointTypesLeft = {DHeavyDotted2Onward}
	, .m_EndpointTypesRight = {ETerminalEndpointType::mc_HeavyDotted2}
	DInsertFileLine
}
,
{
	.m_Combined = str_utf32('┅')
	, .m_Left = gc_Str<str_utf8("┉═╸")>
	, .m_Right = gc_Str<str_utf8("╺")>
	, .m_EndpointTypesLeft = {DHeavyDotted2Onward}
	, .m_EndpointTypesRight = {ETerminalEndpointType::mc_HeavyDotted2}
	DInsertFileLine
}
,
{
	.m_Combined = str_utf32('┅')
	, .m_Left = gc_Str<str_utf8("╶─╌┄┈")>
	, .m_Right = gc_Str<str_utf8("╸")>
	, .m_EndpointTypesLeft = {DLightOnward}
	, .m_EndpointTypesRight = {ETerminalEndpointType::mc_HeavyDotted2}
	DInsertFileLine
}
,
{
	.m_Combined = str_utf32('┅')
	, .m_Left = gc_Str<str_utf8("╴─╌┄┈")>
	, .m_Right = gc_Str<str_utf8("╺")>
	, .m_EndpointTypesLeft = {DLightOnward}
	, .m_EndpointTypesRight = {ETerminalEndpointType::mc_HeavyDotted2}
	DInsertFileLine
}
,
// ┉
{
	.m_Combined = str_utf32('┉')
	, .m_Left = gc_Str<str_utf8("═─╌┄┈╸╺╴╶")>
	, .m_Right = gc_Str<str_utf8("┉")>
	, .m_EndpointTypesLeft = {DHeavyDotted1Onward}
	DInsertFileLine
}
,
{
	.m_Combined = str_utf32('┉')
	, .m_Left = gc_Str<str_utf8("═╺")>
	, .m_Right = gc_Str<str_utf8("╸")>
	, .m_EndpointTypesLeft = {DHeavyDotted1Onward}
	, .m_EndpointTypesRight = {ETerminalEndpointType::mc_HeavyDotted1}
	DInsertFileLine
}
,
{
	.m_Combined = str_utf32('┉')
	, .m_Left = gc_Str<str_utf8("═╸")>
	, .m_Right = gc_Str<str_utf8("╺")>
	, .m_EndpointTypesLeft = {DHeavyDotted1Onward}
	, .m_EndpointTypesRight = {ETerminalEndpointType::mc_HeavyDotted1}
	DInsertFileLine
}
,
{
	.m_Combined = str_utf32('┉')
	, .m_Left = gc_Str<str_utf8("╶─╌┄┈")>
	, .m_Right = gc_Str<str_utf8("╸")>
	, .m_EndpointTypesLeft = {DLightOnward}
	, .m_EndpointTypesRight = {ETerminalEndpointType::mc_HeavyDotted1}
	DInsertFileLine
}
,
{
	.m_Combined = str_utf32('┉')
	, .m_Left = gc_Str<str_utf8("╴─╌┄┈")>
	, .m_Right = gc_Str<str_utf8("╺")>
	, .m_EndpointTypesLeft = {DLightOnward}
	, .m_EndpointTypesRight = {ETerminalEndpointType::mc_HeavyDotted1}
	DInsertFileLine
}
,
