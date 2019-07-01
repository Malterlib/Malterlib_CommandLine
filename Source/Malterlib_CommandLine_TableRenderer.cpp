// Copyright © 2018 Nonna Holding AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#include <Mib/Core/Core>

#include <Mib/CommandLine/AnsiEncoding>
#include <Mib/CommandLine/AnsiEncodingParse>
#include <Mib/Encoding/JSON>
#include <Mib/Encoding/JSONShortcuts>

#include "Malterlib_CommandLine_TableRenderer.h"

namespace NMib::NCommandLine
{
	using namespace NStr;
	using namespace NContainer;
	using namespace NEncoding;

	CTableRenderHelper::CTableRenderHelper(NFunction::TCFunction<void (NStr::CStr const &_Output)> const &_fOutput, EOption _Options, EAnsiEncodingFlag _AnsiFlags)
		: mp_fOutput(_fOutput)
		, mp_Options(_Options)
		, mp_AnsiFlags(_AnsiFlags)
	{
	}

	NEncoding::CEJSON::CKeyValue CTableRenderHelper::fs_OutputTypeOption(EOutputType _Default)
	{
		return "TableType?"_=
			{
				"Names"_= {"--table-type"}
				, "Type"_= NEncoding::fg_UserType("$OneOf", CJSON{"human-readable", "tab-separated", "json", "colored-json"})
				, "Default"_= [&]
				{
					switch (_Default)
					{
						default:
						case EOutputType_HumanReadable: return "human-readable";
						case EOutputType_TabSeparated: return "tab-separated";
						case EOutputType_JSON: return "json";
						case EOutputType_ColoredJSON: return "colored-json";
					}
				}
				()
				, "Description"_= "How to output the table.\n"
					"human-readable    - Display the table rendered with borders.\n"
					"tab-separated     - Output the table as tab separated output suitable for scripting.\n"
					"json              - Output the table as JSON.\n"
					"colored-json      - Output the table as syntax highlighted JSON.\n"
			}
		;
	}

	auto CTableRenderHelper::fs_ParseOutputTypeOption(NEncoding::CEJSON const &_Params) -> EOutputType
	{
		return fs_ParseOutputTypeOption(_Params["TableType"].f_String());
	}

	auto CTableRenderHelper::fs_ParseOutputTypeOption(CStr const &_String) -> EOutputType
	{
		if (_String == "human-readable")
			return EOutputType_HumanReadable;
		else if (_String == "tab-separated")
			return EOutputType_TabSeparated;
		else if (_String == "json")
			return EOutputType_JSON;
		else if (_String == "colored-json")
			return EOutputType_ColoredJSON;

		DMibNeverGetHere;

		return EOutputType_HumanReadable;
	}

	void CTableRenderHelper::f_AddDescription(NStr::CStr const &_Description)
	{
		for (auto &Line : _Description.f_SplitLine())
			mp_DescriptionWidth = fg_Max(mp_DescriptionWidth, (int32)CAnsiEncodingParse::fs_RenderedStrLen(mp_Description.f_Insert(Line)));
	}

	void CTableRenderHelper::f_SetMaxColumnWidth(uint32 _iColumn, uint32 _MaxWidth)
	{
		mp_MaxWidths[_iColumn] = _MaxWidth;
	}

	void CTableRenderHelper::f_RemoveColumn(uint32 _iColumn)
	{
		mp_Headings.f_Remove(_iColumn);
		mp_Widths.f_Remove(_iColumn);
		for (auto &Row : mp_Rows)
			Row.f_Remove(_iColumn);
	}

	void CTableRenderHelper::f_SortColumn(uint32 _iColumn)
	{
		mp_Rows.f_Sort
			(
				[=](auto const &_Left, auto const &_Right)
			 	{
					return _Left[_iColumn].f_CompareLexicographical(_Right[_iColumn]) < 0;
				}
			)
		;
	}

	void CTableRenderHelper::f_SetOptions(EOption _Options)
	{
		mp_Options = _Options;
	}

	void CTableRenderHelper::fp_AddHeading(CStr const &_Heading)
	{
		mp_Widths.f_Insert(CAnsiEncodingParse::fs_RenderedStrLen(mp_Headings.f_Insert(_Heading)));
	}

	void CTableRenderHelper::fp_AddRowColumn(NContainer::TCVector<NContainer::TCVector<NStr::CStr>> &o_RowColumns, CStr const &_Value)
	{
		CStr Value = _Value.f_Replace("\t", "    ");
		mint iColumn = o_RowColumns.f_GetLen();
		auto pMaxWidth = mp_MaxWidths.f_FindEqual(iColumn);
		auto &ColumnWidth = mp_Widths[iColumn];
		auto &ColumnRow = o_RowColumns.f_Insert();

		auto Lines = _Value.f_SplitLine();
		if (pMaxWidth)
		{
			CAnsiEncoding AnsiColor(mp_AnsiFlags);

			for (auto &LongLine : Lines)
			{
				auto NewLines = AnsiColor.f_LineBreak(LongLine, *pMaxWidth);
				for (auto &Line : NewLines)
					ColumnWidth = fg_Max(ColumnWidth, (int32)CAnsiEncodingParse::fs_RenderedStrLen(ColumnRow.f_Insert(Line)));
			}
		}
		else
		{
			for (auto &LongLine : Lines)
			{
				ColumnWidth = fg_Max(ColumnWidth, (int32)CAnsiEncodingParse::fs_RenderedStrLen(ColumnRow.f_Insert(LongLine)));
			}
		}
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

	void CTableRenderHelper::f_Output(NStr::CStr const &_OutputType) const
	{
		f_Output(fs_ParseOutputTypeOption(_OutputType));
	}

	void CTableRenderHelper::f_Output(NEncoding::CEJSON const &_Params) const
	{
		f_Output(fs_ParseOutputTypeOption(_Params));
	}

	void CTableRenderHelper::f_Output(EOutputType _OutputType) const
	{
		if (_OutputType == EOutputType_TabSeparated)
		{
			for (auto &Row : mp_Rows)
			{
				CStr Line;
				for (auto &ColumnLines : Row)
				{
					if (!Line.f_IsEmpty())
						Line += "\t";
					Line += CStr::fs_Join(ColumnLines, "\n").f_EscapeStr("\"\t\r\n", "\"trn");
				}
				Line += "\n";

				fp_Output(Line);
			}

			return;
		}
		else if (_OutputType == EOutputType_JSON || _OutputType == EOutputType_ColoredJSON)
		{
			CJSON Output;

			if (!mp_Description.f_IsEmpty())
			{
				auto &OutputDescription = Output["Description"]["Lines"].f_Array();
				for (auto &DescriptionLine : mp_Description)
					OutputDescription.f_Insert(DescriptionLine);
			}

			if (!mp_MaxWidths.f_IsEmpty())
			{
				auto &OutputMaxWidths = Output["MaxWidths"];
				for (auto &Width : mp_MaxWidths)
					OutputMaxWidths[CStr::fs_ToStr(mp_MaxWidths.fs_GetKey(Width))] = Width;
			}

			auto &OutputHeadings = Output["Headings"].f_Array();
			for (auto &Heading : mp_Headings)
				OutputHeadings.f_Insert(Heading);

			auto &OutputRows = Output["Rows"].f_Array();

			for (auto &Row : mp_Rows)
			{
				auto &OutputRow = OutputRows.f_Insert()["Columns"].f_Array();

				for (auto &ColumnLines : Row)
				{
					auto &OutputRowObject = OutputRow.f_Insert();
					auto &OutputLines = OutputRowObject["Lines"].f_Array();
					OutputLines = ColumnLines;
				}
			}

			if (_OutputType == EOutputType_ColoredJSON)
				fp_Output(Output.f_ToStringColored(mp_AnsiFlags));
			else
				fp_Output(Output.f_ToString());

			return;
		}

		CAnsiEncoding AnsiColor(mp_AnsiFlags);

		auto LineColor = AnsiColor.f_Foreground256(241);

		CUStr LineSeparator = U"{}|{}"_f << LineColor << (char const *)AnsiColor.f_Default();

		bool bAvoidLineSeparators = !!(mp_Options & CTableRenderHelper::EOption_AvoidRowSeparators);

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
			for (auto &MaxWidth : mp_Widths)
				TotalWidth += MaxWidth + 2;
			CUStr TopLine;
			{
				TopLine = ((f_IsRounded() && !bHasDescription) ? U"{}/"_f : U"{}|"_f) << LineColor;
				for (auto &MaxWidth : mp_Widths)
					TopLine += U"{sf¯,sj*}|"_f << "" << (MaxWidth + 2);
				if (f_IsRounded())
					TopLine[TopLine.f_GetLen() - 1] = '\\';
				TopLine += AnsiColor.f_Default();
			}
			CUStr MiddleLine;
			{
				MiddleLine = U"{}|"_f << LineColor;
				for (auto &MaxWidth : mp_Widths)
					MiddleLine += U"{sf-,sj*}|"_f << "" << (MaxWidth + 2);
				MiddleLine += AnsiColor.f_Default();
			}
			CUStr BottomLine;
			{
				BottomLine = (f_IsRounded() ? U"{}\\"_f : U"{}|"_f) << LineColor;
				for (auto &MaxWidth : mp_Widths)
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
					auto &MaxWidth = mp_Widths[iColumn];
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

			bool bWasMultiLine = true;

			for (auto &Row : mp_Rows)
			{
				mint MaxLines = 0;
				for (auto &ColumnLines : Row)
					MaxLines = fg_Max(MaxLines, ColumnLines.f_GetLen());

				if (MaxLines == 0)
					continue;

				if (!bAvoidLineSeparators || bWasMultiLine)
					fp_Output("{}\n"_f << MiddleLine);

				for (mint iLine = 0; iLine < MaxLines; ++iLine)
				{
					CUStr Line = LineSeparator;
					mint iColumn = 0;
					for (auto &ColumnLines : Row)
					{
						auto &MaxWidth = mp_Widths[iColumn];
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
				bWasMultiLine = MaxLines > 1;
			}

			fp_Output("{}\n\n"_f << BottomLine);
		}
	}
}

#ifndef DMibPNoShortCuts
	using namespace NMib::NCommandLine;
#endif
