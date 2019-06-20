// Copyright © 2018 Nonna Holding AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#include <Mib/Core/Core>

#include <Mib/CommandLine/AnsiEncoding>
#include <Mib/CommandLine/AnsiEncodingParse>

#include "Malterlib_CommandLine_TableRenderer.h"

namespace NMib::NCommandLine
{
	using namespace NStr;
	using namespace NContainer;

	CTableRenderHelper::CTableRenderHelper(NFunction::TCFunction<void (NStr::CStr const &_Output)> const &_fOutput, EOption _Options, EAnsiEncodingFlag _AnsiFlags)
		: mp_fOutput(_fOutput)
		, mp_Options(_Options)
		, mp_AnsiFlags(_AnsiFlags)
	{
	}

	void CTableRenderHelper::f_AddDescription(NStr::CStr const &_Description)
	{
		for (auto &Line : _Description.f_SplitLine())
			mp_DescriptionWidth = fg_Max(mp_DescriptionWidth, (int32)CAnsiEncodingParse::fs_RenderedStrLen(mp_Description.f_Insert(Line)));
	}

	void CTableRenderHelper::fp_AddHeading(CStr const &_Heading)
	{
		mp_Widths.f_Insert(CAnsiEncodingParse::fs_RenderedStrLen(mp_Headings.f_Insert(_Heading)));
	}

	void CTableRenderHelper::fp_AddRowColumn(NContainer::TCVector<NContainer::TCVector<NStr::CStr>> &o_RowColumns, CStr const &_Value)
	{
		CStr Value = _Value.f_Replace("\t", "    ");
		mint iColumn = o_RowColumns.f_GetLen();
		auto &MaxWidth = mp_MaxWidths[iColumn];
		auto &ColumnRow = o_RowColumns.f_Insert();
		for (auto &Line : Value.f_SplitLine())
			MaxWidth = fg_Max(MaxWidth, (int32)CAnsiEncoding::fs_RenderedStrLen(ColumnRow.f_Insert(Line)));
	}

	void CTableRenderHelper::fp_Output(NStr::CStr const &_String) const
	{
		mp_fOutput(_String);
	}

	bool CTableRenderHelper::f_IsRounded() const
	{
		return (mp_Options & EOption_Rounded) != 0;
	}

	bool CTableRenderHelper::f_IsColor() const
	{
		return (mp_AnsiFlags & EAnsiEncodingFlag_Color) != 0;
	}

	void CTableRenderHelper::f_Output() const
	{
		CAnsiEncoding AnsiColor(mp_AnsiFlags);

		auto LineColor = AnsiColor.f_Foreground256(241);

		CUStr LineSeparator = U"{}|{}"_f << LineColor << (char const *)AnsiColor.f_Default();

		CUStr Description;
		bool bHasDescription = !mp_Description.f_IsEmpty();
		if (bHasDescription)
		{
			Description += (f_IsRounded() ? U"{}/"_f : U"{}|"_f) << LineColor;
			Description += U"{sf¯,sj*}"_f << "" << (mp_DescriptionWidth + 2);
			Description += (f_IsRounded() ? U"\\{}\n"_f : U"{}|"_f) << AnsiColor.f_Default();

			auto fAddLine = [&](CUStr const &_Line)
				{
					Description += U"{} {}{}{sj*,sf ,a-}{}{} {}\n"_f
						<< LineSeparator
						<< AnsiColor.f_Bold()
						<< AnsiColor.f_Foreground256(11)
						<< _Line
						<< (mp_DescriptionWidth + (_Line.f_GetLen() - CAnsiEncodingParse::fs_RenderedStrLen(_Line)))
						<< AnsiColor.f_Default()
						<< LineColor
						<< LineSeparator
					;
				}
			;

			for (auto &Line : mp_Description)
				fAddLine(Line);

			fAddLine("");
		}
		{
			mint TotalWidth = 2;
			for (auto &MaxWidth : mp_MaxWidths)
				TotalWidth += MaxWidth + 2;
			CUStr TopLine;
			{
				TopLine = ((f_IsRounded() && !bHasDescription) ? U"{}/"_f : U"{}|"_f) << LineColor;
				for (auto &MaxWidth : mp_MaxWidths)
					TopLine += U"{sf¯,sj*}|"_f << "" << (MaxWidth + 2);
				if (f_IsRounded())
					TopLine[TopLine.f_GetLen() - 1] = '\\';
				TopLine += AnsiColor.f_Default();
			}
			CUStr MiddleLine;
			{
				MiddleLine = U"{}|"_f << LineColor;
				for (auto &MaxWidth : mp_MaxWidths)
					MiddleLine += U"{sf-,sj*}|"_f << "" << (MaxWidth + 2);
				MiddleLine += AnsiColor.f_Default();
			}
			CUStr BottomLine;
			{
				BottomLine = (f_IsRounded() ? U"{}\\"_f : U"{}|"_f) << LineColor;
				for (auto &MaxWidth : mp_MaxWidths)
					BottomLine += U"{sf_,sj*}|"_f << "" << (MaxWidth + 2);
				if (f_IsRounded())
					BottomLine[BottomLine.f_GetLen() - 1] = '/';
				BottomLine += AnsiColor.f_Default();
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
						<< (MaxWidth + (Heading.f_GetLen() - CAnsiEncodingParse::fs_RenderedStrLen(Heading)))
						<< LineSeparator
						<< (char const *)AnsiColor.f_Bold()
						<< (char const *)AnsiColor.f_Default()
					;
				}
				fp_Output("\n{}{}\n{}\n"_f << Description << TopLine << Line);
			}

			for (auto &Row : mp_Rows)
			{
				mint MaxLines = 0;
				for (auto &ColumnLines : Row)
					MaxLines = fg_Max(MaxLines, ColumnLines.f_GetLen());

				if (MaxLines == 0)
					continue;

				fp_Output("{}\n"_f << MiddleLine);

				for (mint iLine = 0; iLine < MaxLines; ++iLine)
				{
					CUStr Line = LineSeparator;
					mint iColumn = 0;
					for (auto &ColumnLines : Row)
					{
						auto &MaxWidth = mp_MaxWidths[iColumn];
						++iColumn;
						CStr SourceLine;
						if (ColumnLines.f_IsPosValid(iLine))
							SourceLine = ColumnLines[iLine];

						Line += U" {sj*,sf ,a-} {}"_f
							<< SourceLine
							<< (MaxWidth + (SourceLine.f_GetLen() - CAnsiEncodingParse::fs_RenderedStrLen(SourceLine)))
							<< LineSeparator
						;
					}
					fp_Output("{}\n"_f << Line);
				}
			}

			fp_Output("{}\n\n"_f << BottomLine);
		}
	}
}

#ifndef DMibPNoShortCuts
	using namespace NMib::NCommandLine;
#endif
