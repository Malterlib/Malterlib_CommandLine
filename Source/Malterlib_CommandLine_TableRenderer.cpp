 // Copyright © 2018 Nonna Holding AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#include <Mib/Core/Core>

#include <Mib/CommandLine/AnsiEncoding>
#include <Mib/CommandLine/AnsiEncodingParse>
#include <Mib/Encoding/Json>
#include <Mib/Encoding/JsonShortcuts>

#include "Malterlib_CommandLine_TableRenderer.h"

namespace NMib::NCommandLine
{
	using namespace NStr;
	using namespace NContainer;
	using namespace NEncoding;

	CTableRenderHelper::CTableRenderHelper(NFunction::TCFunction<void (NStr::CStr const &_Output)> const &_fOutput, EOption _Options, EAnsiEncodingFlag _AnsiFlags, uint32 _AvailableWidth)
		: mp_fOutput(_fOutput)
		, mp_Options(_Options)
		, mp_AnsiFlags(_AnsiFlags)
		, mp_AvailableWidth(_AvailableWidth)
	{
	}

	NEncoding::CEJsonOrdered::CKeyValue CTableRenderHelper::fs_OutputTypeOption(EOutputType _Default)
	{
		return "TableType?"_o=
			{
				"Names"_o= _o["--table-type"]
				, "Type"_o= NEncoding::fg_UserTypeOrdered("$OneOf", _jo["human-readable", "tab-separated", "json", "colored-json"])
				, "Default"_o= [&]
				{
					switch (_Default)
					{
						default:
						case EOutputType_HumanReadable: return "human-readable";
						case EOutputType_TabSeparated: return "tab-separated";
						case EOutputType_Json: return "json";
						case EOutputType_ColoredJson: return "colored-json";
					}
				}
				()
				, "Description"_o= "How to output the table.\n"
					"human-readable    - Display the table rendered with borders.\n"
					"tab-separated     - Output the table as tab separated output suitable for scripting.\n"
					"json              - Output the table as Json.\n"
					"colored-json      - Output the table as syntax highlighted Json.\n"
			}
		;
	}

	auto CTableRenderHelper::fs_ParseOutputTypeOption(NEncoding::CEJsonSorted const &_Params) -> EOutputType
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
			return EOutputType_Json;
		else if (_String == "colored-json")
			return EOutputType_ColoredJson;

		DMibNeverGetHere;

		return EOutputType_HumanReadable;
	}

	void CTableRenderHelper::f_ForceRowSeparator()
	{
		mp_RowSeparators[mp_Rows.f_GetLen()];
	}

	void CTableRenderHelper::f_AddDescription(NStr::CStr const &_Description)
	{
		CAnsiEncoding AnsiColor(mp_AnsiFlags);

		auto Lines = AnsiColor.f_LineBreak(_Description, TCLimitsInt<mint>::mc_Max, CAnsiEncoding::EWordWrap_Character);

		for (auto &Line : Lines)
			mp_DescriptionWidth = fg_Max(mp_DescriptionWidth, (int32)CAnsiEncodingParse::fs_RenderedStrLen(mp_Description.f_Insert(Line.m_String)));
	}

	void CTableRenderHelper::f_SetMaxColumnWidth(uint32 _iColumn, uint32 _MaxWidth)
	{
		mp_MaxWidths[_iColumn] = _MaxWidth;
	}

	void CTableRenderHelper::f_SetMinColumnWidth(uint32 _iColumn, uint32 _MinWidth)
	{
		mp_MinWidths[_iColumn] = _MinWidth;
	}

	void CTableRenderHelper::f_SetAlignRight(uint32 _iColumn)
	{
		mp_AlignRight[_iColumn] = true;
	}

	void CTableRenderHelper::f_SetPrefix(NStr::CStr const &_Prefix)
	{
		mp_Prefix = _Prefix;
	}

	void CTableRenderHelper::f_RemoveColumn(uint32 _iColumn)
	{
		mp_Headings.f_Remove(_iColumn);
		mp_Widths.f_Remove(_iColumn);
		mp_AlignRight.f_Remove(_iColumn);
		for (auto &Row : mp_Rows)
			Row.f_Remove(_iColumn);
	}

	void CTableRenderHelper::f_SortColumns(TCVector<uint32> const &_Columns)
	{
		if (_Columns.f_IsEmpty())
			return;

		mp_Rows.f_Sort
			(
				[&](auto const &_Left, auto const &_Right) -> COrdering_Strong
				{
					for (auto &iColumn : _Columns)
					{
						auto Compare = _Left[iColumn].f_CompareLexicographical(_Right[iColumn]) <=> 0;
						if (Compare != 0)
							return Compare;
					}

					return COrdering_Strong::equal;
				}
			)
		;
	}

	void CTableRenderHelper::f_SortColumn(uint32 _iColumn)
	{
		mp_Rows.f_Sort
			(
				[=](auto const &_Left, auto const &_Right)
				{
					return _Left[_iColumn].f_CompareLexicographical(_Right[_iColumn]) <=> 0;
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
		mp_AlignRight.f_Insert();
	}

	void CTableRenderHelper::fp_AddRowColumn(NContainer::TCVector<NContainer::TCVector<NStr::CStr>> &o_RowColumns, CStr const &_Value)
	{
		CStr Value = _Value.f_Replace("\t", "  ");
		mint iColumn = o_RowColumns.f_GetLen();
		auto &ColumnWidth = mp_Widths[iColumn];
		auto &ColumnRow = o_RowColumns.f_Insert();

		CAnsiEncoding AnsiColor(mp_AnsiFlags);

		auto Lines = AnsiColor.f_LineBreak(Value, TCLimitsInt<mint>::mc_Max, CAnsiEncoding::EWordWrap_Character);

		for (auto &LongLine : Lines)
			ColumnWidth = fg_Max(ColumnWidth, (uint32)CAnsiEncodingParse::fs_RenderedStrLen(ColumnRow.f_Insert(LongLine.m_String)));
	}

	void CTableRenderHelper::fp_Output(NStr::CStr const &_String) const
	{
		if (mp_Prefix)
			mp_fOutput(_String.f_Indent(mp_Prefix, true, false));
		else
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

	void CTableRenderHelper::f_Output(NStr::CStr const &_OutputType)
	{
		f_Output(fs_ParseOutputTypeOption(_OutputType));
	}

	void CTableRenderHelper::f_Output(NEncoding::CEJsonSorted const &_Params)
	{
		f_Output(fs_ParseOutputTypeOption(_Params));
	}

	void CTableRenderHelper::f_ReverseRows()
	{
		mp_Rows = mp_Rows.f_Reverse();

		mint LastIndex = mp_Rows.f_GetLen() - 1;

		NContainer::TCSet<mint> NewRowSeparators;
		for (auto &SeparatorIndex : mp_RowSeparators)
			NewRowSeparators[LastIndex - SeparatorIndex];

		mp_RowSeparators = fg_Move(NewRowSeparators);
	}

	void CTableRenderHelper::f_MergeColumnWidths(CTableRenderHelper const &_Other)
	{
		if (mp_Widths.f_IsEmpty())
			mp_Widths = _Other.mp_Widths;
		else
		{
			DMibRequire(mp_Widths.f_GetLen() == _Other.mp_Widths.f_GetLen());
			mint nColumns = mp_Widths.f_GetLen();
			for (mint iColumn = 0; iColumn < nColumns; ++iColumn)
			{
				auto Max = fg_Max(mp_Widths[iColumn], _Other.mp_Widths[iColumn]);
				mp_Widths[iColumn] = Max;
			}
		}
	}

	void CTableRenderHelper::f_Output(EOutputType _OutputType)
	{
		if (mp_pColumnsHelper)
		{
			TCVector<uint32> SortByColumnIds;
			for (auto &ColumnName : mp_pColumnsHelper->mp_SortByColumns)
			{
				auto *pColumnIndex = mp_pColumnsHelper->mp_HeadingIndices.f_FindEqual(ColumnName);
				if (!pColumnIndex)
					continue;
				SortByColumnIds.f_Insert(*pColumnIndex);
			}

			f_SortColumns(SortByColumnIds);

			while (auto pVerbosityLevel = mp_pColumnsHelper->mp_VerboseHeadings.f_FindLargest())
			{
				if (*pVerbosityLevel > mp_pColumnsHelper->mp_Verbosity)
					f_RemoveColumn(mp_pColumnsHelper->mp_VerboseHeadings.fs_GetKey(*pVerbosityLevel));
				mp_pColumnsHelper->mp_VerboseHeadings.f_Remove(pVerbosityLevel);
			}
		}

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
		else if (_OutputType == EOutputType_Json || _OutputType == EOutputType_ColoredJson)
		{
			CJsonSorted Output;

			if (!mp_Description.f_IsEmpty())
			{
				auto &OutputDescription = Output["Description"]["Lines"].f_Array();

				for (auto &DescriptionLine : mp_Description)
					OutputDescription.f_Insert(DescriptionLine);
			}

			if (!mp_MaxWidths.f_IsEmpty())
			{
				auto &OutputMaxWidths = Output["MaxWidths"] = EJsonType_Object;

				for (auto &Width : mp_MaxWidths)
					OutputMaxWidths[CStr::fs_ToStr(mp_MaxWidths.fs_GetKey(Width))] = Width;
			}

			{
				auto &OutputAlignRight = Output["AlignRight"] = EJsonType_Array;
				OutputAlignRight.f_SetLen(mp_AlignRight.f_GetLen());

				for (auto &bAlign : mp_AlignRight)
				{
					auto iAlign = &bAlign - mp_AlignRight.f_GetArray();
					OutputAlignRight[iAlign] = bAlign;
				}
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

			if (_OutputType == EOutputType_ColoredJson)
				fp_Output(Output.f_ToStringColored(mp_AnsiFlags));
			else
				fp_Output(Output.f_ToString());

			return;
		}

		CAnsiEncoding AnsiColor(mp_AnsiFlags);

		bool bUseBoxDrawing = !!(mp_AnsiFlags & EAnsiEncodingFlag_BoxDrawing);

		CUStr LineColor;
		if (bUseBoxDrawing)
			LineColor = AnsiColor.f_Foreground256(244);
		else
			LineColor = AnsiColor.f_Foreground256(241);

		CUStr LineSeparator;
		if (bUseBoxDrawing)
			LineSeparator = U"{}│{}"_f << LineColor << (char const *)AnsiColor.f_Default();
		else
			LineSeparator = U"{}|{}"_f << LineColor << (char const *)AnsiColor.f_Default();

		bool bAvoidLineSeparators = !!(mp_Options & CTableRenderHelper::EOption_AvoidRowSeparators);
		bool bNoExtraLines = !!(mp_Options & CTableRenderHelper::EOption_NoExtraLines);
		bool bNoHeadings = !!(mp_Options & CTableRenderHelper::EOption_NoHeadings);

		CUStr Description;
		bool bHasDescription = !mp_Description.f_IsEmpty();
		if (bHasDescription)
		{
			if (bUseBoxDrawing)
			{
				Description += (f_IsRounded() ? U"{}╭"_f : U"{}┌"_f) << LineColor;
				Description += U"{sf─,sj*}"_f << "" << (mp_DescriptionWidth + 2);
				Description += (f_IsRounded() ? U"╮{}\n"_f : U"┐{}\n"_f) << AnsiColor.f_Default();
			}
			else
			{
				Description += (f_IsRounded() ? U"{}/"_f : U"{}|"_f) << LineColor;
				Description += U"{sf¯,sj*}"_f << "" << (mp_DescriptionWidth + 2);
				Description += (f_IsRounded() ? U"\\{}\n"_f : U"|{}\n"_f) << AnsiColor.f_Default();
			}

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

			if (!bUseBoxDrawing)
				fAddLine("");
		}
		{
			TCVector<uint32> LimitedWidths;
			mint nColumns = mp_Widths.f_GetLen();
			LimitedWidths.f_SetLen(nColumns);
			for (mint iColumn = 0; iColumn < nColumns; ++iColumn)
			{
				uint32 MaxWidth = TCLimitsInt<uint32>::mc_Max;
				auto pMaxWidth = mp_MaxWidths.f_FindEqual(iColumn);
				if (pMaxWidth)
					MaxWidth = *pMaxWidth;

				LimitedWidths[iColumn] = fg_Min(mp_Widths[iColumn], MaxWidth);

				auto pMinWidth = mp_MinWidths.f_FindEqual(iColumn);
				if (pMinWidth)
					LimitedWidths[iColumn] = fg_Max(LimitedWidths[iColumn], *pMinWidth);
			}

			uint32 TotalWidth = 1;
			for (auto &Width : LimitedWidths)
				TotalWidth += Width + 3;

			while (TotalWidth >= mp_AvailableWidth)
			{
				uint32 HighestWidth = 0;
				mint iHighestWidth = TCLimitsInt<mint>::mc_Max;
				for (mint iColumn = 0; iColumn < nColumns; ++iColumn)
				{
					auto &Width = LimitedWidths[iColumn];
					if (Width > HighestWidth && Width > 10)
					{
						HighestWidth = Width;
						iHighestWidth = iColumn;
					}
				}

				if (iHighestWidth == TCLimitsInt<mint>::mc_Max)
					break; // Failed

				--LimitedWidths[iHighestWidth];
				--TotalWidth;
			}

			CUStr TopLine;
			if (bUseBoxDrawing)
			{
				if (bHasDescription)
					TopLine = U"├";
				else
					TopLine = (f_IsRounded() ? U"╭" : U"┌");

				for (auto &MaxWidth : LimitedWidths)
					TopLine += U"{sf─,sj*}┬"_f << "" << (MaxWidth + 2);

				TopLine[TopLine.f_GetLen() - 1] = f_IsRounded() ? U'╮' : U'┐';

				if (bHasDescription && (TopLine.f_GetLen() > mp_DescriptionWidth + 4))
				{
					if (TopLine[mp_DescriptionWidth + 3] == U'┬')
						TopLine[mp_DescriptionWidth + 3] = U'┼';
					else
						TopLine[mp_DescriptionWidth + 3] = U'┴';
				}

				TopLine = LineColor + TopLine + AnsiColor.f_Default();
			}
			else
			{
				TopLine = ((f_IsRounded() && !bHasDescription) ? U"{}/"_f : U"{}|"_f) << LineColor;
				for (auto &MaxWidth : LimitedWidths)
					TopLine += U"{sf¯,sj*}|"_f << "" << (MaxWidth + 2);
				if (f_IsRounded())
					TopLine[TopLine.f_GetLen() - 1] = '\\';
				TopLine += AnsiColor.f_Default();
			}

			CUStr MiddleLine;
			if (bUseBoxDrawing)
			{
				MiddleLine = U"{}├"_f << LineColor;
				for (auto &MaxWidth : LimitedWidths)
					MiddleLine += U"{sf─,sj*}┼"_f << "" << (MaxWidth + 2);
				MiddleLine[MiddleLine.f_GetLen() - 1] = U'┤';
				MiddleLine += AnsiColor.f_Default();
			}
			else
			{
				MiddleLine = U"{}|"_f << LineColor;
				for (auto &MaxWidth : LimitedWidths)
					MiddleLine += U"{sf-,sj*}|"_f << "" << (MaxWidth + 2);
				MiddleLine += AnsiColor.f_Default();
			}

			CUStr BottomLine;
			if (bUseBoxDrawing)
			{
				BottomLine = (f_IsRounded() ? U"{}╰"_f : U"{}└"_f) << LineColor;
				for (auto &MaxWidth : LimitedWidths)
					BottomLine += U"{sf─,sj*}┴"_f << "" << (MaxWidth + 2);
				BottomLine[BottomLine.f_GetLen() - 1] = f_IsRounded() ? U'╯' :U'┘';
				BottomLine += AnsiColor.f_Default();
			}
			else
			{
				BottomLine = (f_IsRounded() ? U"{}\\"_f : U"{}|"_f) << LineColor;
				for (auto &MaxWidth : LimitedWidths)
					BottomLine += U"{sf_,sj*}|"_f << "" << (MaxWidth + 2);
				if (f_IsRounded())
					BottomLine[BottomLine.f_GetLen() - 1] = '/';
				BottomLine += AnsiColor.f_Default();
			}

			if (bNoHeadings)
				fp_Output("{}{}{}\n"_f << ((bUseBoxDrawing || bNoExtraLines) ? "" : "\n") << Description << TopLine);
			else
			{
				mint iColumn = 0;
				CUStr Line = LineSeparator;
				for (auto &Heading : mp_Headings)
				{
					auto &MaxWidth = LimitedWidths[iColumn];

					ch32 const *pFormatString = U" {3}{sj*,sf ,a-}{4} {}";
					if (mp_AlignRight[iColumn])
						pFormatString = U" {3}{sj*,sf ,a+}{4} {}";

					Line += CUStr::CFormat(pFormatString)
						<< Heading
						<< (MaxWidth + (Heading.f_GetLen() - CAnsiEncodingParse::fs_RenderedStrLen(Heading)))
						<< LineSeparator
						<< (char const *)AnsiColor.f_Bold()
						<< (char const *)AnsiColor.f_Default()
					;

					++iColumn;
				}
				fp_Output("{}{}{}\n{}\n"_f << ((bUseBoxDrawing || bNoExtraLines) ? "" : "\n") << Description << TopLine << Line);
			}

			bool bWasMultiLine = !bNoHeadings;

			NContainer::TCVector<NContainer::TCVector<NContainer::TCVector<NStr::CStr>>> LinebrokenRows;
			for (auto &Row : mp_Rows)
			{
				auto &OutRow = LinebrokenRows.f_Insert();
				for (auto &ColumnLines : Row)
				{
					auto &OutColumnLines = OutRow.f_Insert();

					auto iColumn = &ColumnLines - Row.f_GetArray();
					auto &RealWidth = mp_Widths[iColumn];
					auto &MaxWidth = LimitedWidths[iColumn];

					if (RealWidth <= MaxWidth)
					{
						OutColumnLines = ColumnLines;
						continue;
					}

					for (auto &LongLine : ColumnLines)
					{
						auto NewLines = AnsiColor.f_LineBreak(LongLine, MaxWidth);
						for (auto &Line : NewLines)
							OutColumnLines.f_Insert(Line.m_String);
					}
				}
			}

			for (auto &Row : LinebrokenRows)
			{
				mint iRow = &Row - LinebrokenRows.f_GetArray();

				mint MaxLines = 0;
				for (auto &ColumnLines : Row)
					MaxLines = fg_Max(MaxLines, ColumnLines.f_GetLen());

				if (MaxLines == 0)
					continue;

				if (!bAvoidLineSeparators || bWasMultiLine || (bAvoidLineSeparators && !bWasMultiLine && MaxLines > 1) || mp_RowSeparators.f_Exists(iRow))
					fp_Output("{}\n"_f << MiddleLine);

				for (mint iLine = 0; iLine < MaxLines; ++iLine)
				{
					CUStr Line = LineSeparator;
					mint iColumn = 0;
					for (auto &ColumnLines : Row)
					{
						auto &MaxWidth = LimitedWidths[iColumn];

						ch32 const *pFormatString = U" {sj*,sf ,a-} {}";
						if (mp_AlignRight[iColumn])
							pFormatString = U" {sj*,sf ,a+} {}";

						CUStr SourceLine;
						if (ColumnLines.f_IsPosValid(iLine))
							SourceLine = ColumnLines[iLine];

						Line += CUStr::CFormat(pFormatString)
							<< SourceLine
							<< (MaxWidth + (SourceLine.f_GetLen() - CAnsiEncodingParse::fs_RenderedStrLen(SourceLine)))
							<< LineSeparator
						;

						++iColumn;
					}
					fp_Output("{}\n"_f << Line);
				}
				bWasMultiLine = MaxLines > 1;
			}

			fp_Output("{}\n{}"_f << BottomLine << (bNoExtraLines ? "" : "\n"));
		}
	}

	void CTableRenderHelper::f_AddHeadingsVector(NContainer::TCVector<NStr::CStr> const &_Headings)
	{
		DMibRequire(mp_Headings.f_IsEmpty());

		for (auto &Heading : _Headings)
			fp_AddHeading(Heading);
	}

	CTableRenderHelper::CColumnHelper::CColumnHelper(uint32 _Verbosity)
		: mp_Verbosity(_Verbosity)
	{
	}

	void CTableRenderHelper::CColumnHelper::f_AddHeading(NStr::CStr const &_Name, uint32 _Verbosity)
	{
		auto HeadingIndex = mp_Headings.f_GetLen();
		mp_HeadingIndices[_Name] = HeadingIndex;
		if (_Verbosity)
			mp_VerboseHeadings[HeadingIndex] = _Verbosity;

		mp_Headings.f_Insert(_Name);
	}

	void CTableRenderHelper::CColumnHelper::f_SetSortByColumns(NContainer::TCVector<NStr::CStr> const &_SortByColumns)
	{
		mp_SortByColumns = _SortByColumns;
	}

	void CTableRenderHelper::CColumnHelper::f_SetVerbose(NStr::CStr const &_Heading, uint32 _Verbosity)
	{
		DMibRequire(mp_HeadingIndices.f_FindEqual(_Heading));
		auto &Index = fg_Const(mp_HeadingIndices)[_Heading];
		if (_Verbosity)
			mp_VerboseHeadings[Index] = _Verbosity;
		else
			mp_VerboseHeadings.f_Remove(Index);
	}

	void CTableRenderHelper::f_AddHeadings(CColumnHelper *_pHelper)
	{
		mp_pColumnsHelper = _pHelper;
		f_AddHeadingsVector(mp_pColumnsHelper->mp_Headings);
	}

	void CTableRenderHelper::f_AddRowVector(NContainer::TCVector<NStr::CStr> const &_RowColumns)
	{
		DMibRequire(!mp_Widths.f_IsEmpty());
		DMibRequire(_RowColumns.f_GetLen() == mp_Headings.f_GetLen());

		NContainer::TCVector<NContainer::TCVector<NStr::CStr>> RowColumns;
		for (auto &Value : _RowColumns)
			fp_AddRowColumn(RowColumns, Value);

		mp_Rows.f_Insert(RowColumns);
	}
}

#ifndef DMibPNoShortCuts
	using namespace NMib::NCommandLine;
#endif
