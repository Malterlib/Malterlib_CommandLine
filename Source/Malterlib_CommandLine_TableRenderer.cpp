// Copyright © 2018 Nonna Holding AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#include <Mib/CommandLine/AnsiEncoding>

#include "Malterlib_CommandLine_TableRenderer.h"

namespace NMib::NCommandLine
{
	using namespace NStr;
	using namespace NContainer;

	CTableRenderHelper::CTableRenderHelper(NFunction::TCFunction<void (NStr::CStr const &_Output)> const &_fOutput, EOption _Options)
		: mp_fOutput(_fOutput)
		, mp_Options(_Options)
	{
	}

	void CTableRenderHelper::fp_AddHeading(CStr const &_Heading)
	{
		mp_MaxWidths.f_Insert(CAnsiEncoding::fs_RenderedStrLen(mp_Headings.f_Insert(_Heading)));
	}

	void CTableRenderHelper::fp_AddRowColumn(TCVector<CStr> &o_RowColumns, CStr const &_Value)
	{
		mint iColumn = o_RowColumns.f_GetLen();
		auto &MaxWidth = mp_MaxWidths[iColumn];
		MaxWidth = fg_Max(MaxWidth, (int32)CAnsiEncoding::fs_RenderedStrLen(o_RowColumns.f_Insert(_Value)));
	}

	bool CTableRenderHelper::f_IsRounded() const
	{
		return (mp_Options & EOption_Rounded) != 0;
	}

	bool CTableRenderHelper::f_IsColor() const
	{
		return (mp_Options & EOption_Color) != 0;
	}

	void CTableRenderHelper::f_Output() const
	{
		mint TotalWidth = 2;
		for (auto &MaxWidth : mp_MaxWidths)
			TotalWidth += MaxWidth + 2;
		auto LineColor = CAnsiEncoding::fs_Foreground256(241);
		CUStr LineSeparator = U"{}|{}"_f << LineColor << (char const *)CAnsiEncoding::ms_Default;
		CUStr TopLine;
		{
			TopLine = (f_IsRounded() ? U"{}/"_f : U"{}|"_f) << LineColor;
			for (auto &MaxWidth : mp_MaxWidths)
				TopLine += U"{sf¯,sj*}|"_f << "" << (MaxWidth + 2);
			if (f_IsRounded())
				TopLine[TopLine.f_GetLen() - 1] = '\\';
			TopLine += CAnsiEncoding::ms_Default;
		}
		CUStr MiddleLine;
		{
			MiddleLine = U"{}|"_f << LineColor;
			for (auto &MaxWidth : mp_MaxWidths)
				MiddleLine += U"{sf-,sj*}|"_f << "" << (MaxWidth + 2);
			MiddleLine += CAnsiEncoding::ms_Default;
		}
		CUStr BottomLine;
		{
			BottomLine = (f_IsRounded() ? U"{}\\"_f : U"{}|"_f) << LineColor;
			for (auto &MaxWidth : mp_MaxWidths)
				BottomLine += U"{sf_,sj*}|"_f << "" << (MaxWidth + 2);
			if (f_IsRounded())
				BottomLine[BottomLine.f_GetLen() - 1] = '/';
			BottomLine += CAnsiEncoding::ms_Default;
		}

		{
			mint iColumn = 0;
			CUStr Line = LineSeparator;
			for (auto &Heading : mp_Headings)
			{
				auto &MaxWidth = mp_MaxWidths[iColumn];
				++iColumn;
				Line += U" {3}{sj*,sf ,a-}{4} {}"_f
					<< Heading
					<< (MaxWidth + (Heading.f_GetLen() - CAnsiEncoding::fs_RenderedStrLen(Heading)))
					<< LineSeparator
					<< (char const *)CAnsiEncoding::ms_Bold
					<< (char const *)CAnsiEncoding::ms_Default
				;
			}
			mp_fOutput("\n{}\n{}\n"_f << TopLine << Line);
		}

		for (auto &Row : mp_Rows)
		{
			mint iColumn = 0;
			CUStr Line = LineSeparator;
			for (auto &ColumnData : Row)
			{
				auto &MaxWidth = mp_MaxWidths[iColumn];
				++iColumn;
				Line += U" {sj*,sf ,a-} {}"_f
					<< ColumnData
					<< (MaxWidth + (ColumnData.f_GetLen() - CAnsiEncoding::fs_RenderedStrLen(ColumnData)))
					<< LineSeparator
				;
			}
			mp_fOutput("{}\n{}\n"_f << MiddleLine << Line);
		}


		mp_fOutput("{}\n\n"_f << BottomLine);
	}
}

#ifndef DMibPNoShortCuts
	using namespace NMib::NCommandLine;
#endif
