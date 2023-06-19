// Copyright © 2018 Nonna Holding AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

#include <Mib/Function/Function>
#include <Mib/Encoding/EJSON>

namespace NMib::NCommandLine
{
	struct CTableRenderHelper
	{
		enum EOption : uint32
		{
			EOption_None = 0
			, EOption_Rounded = DMibBit(0)
			, EOption_AvoidRowSeparators = DMibBit(1)
			, EOption_NoExtraLines = DMibBit(2)
		};

		enum EOutputType : uint32
		{
			EOutputType_HumanReadable = 0
			, EOutputType_TabSeparated
			, EOutputType_JSON
			, EOutputType_ColoredJSON
		};

		struct CColumnHelper
		{
			CColumnHelper(uint32 _Verbosity);

			void f_AddHeading(NStr::CStr const &_Name, uint32 _Verbosity);
			void f_SetVerbose(NStr::CStr const &_Heading, uint32 _Verbosity = 1);

		private:
			friend struct CTableRenderHelper;

			uint32 mp_Verbosity = 0;
			NContainer::TCVector<NStr::CStr> mp_Headings;
			NContainer::TCMap<mint, uint32> mp_VerboseHeadings;
			NContainer::TCMap<NStr::CStr, mint> mp_HeadingIndices;
		};

		CTableRenderHelper(NFunction::TCFunction<void (NStr::CStr const &_Output)> const &_fOutput, EOption _Options, EAnsiEncodingFlag _AnsiFlags, uint32 _AvailableWidth);
		CTableRenderHelper(CTableRenderHelper &&) = default;

		void f_AddDescription(NStr::CStr const &_Description);
		template <typename ...tfp_CString>
		void f_AddHeadings(tfp_CString &&...p_Headings)
			requires (sizeof...(p_Headings) >= 1)
		;
		void f_AddHeadingsVector(NContainer::TCVector<NStr::CStr> const &_Headings);
		void f_AddHeadings(CColumnHelper *_pHelper);

		template <typename ...tfp_CString>
		void f_AddRow(tfp_CString &&...p_RowColumns)
			requires (sizeof...(p_RowColumns) >= 1)
		;
		void f_AddRowVector(NContainer::TCVector<NStr::CStr> const &_RowColumns);
		void f_MergeColumnWidths(CTableRenderHelper const &_Other);
		void f_ForceRowSeparator();
		void f_SetMaxColumnWidth(uint32 _iColumn, uint32 _MaxWidth);
		void f_SetAlignRight(uint32 _iColumn);
		void f_SetPrefix(NStr::CStr const &_Prefix);
		void f_RemoveColumn(uint32 _iColumn);
		void f_SortColumn(uint32 _iColumn);
		void f_SetOptions(EOption _Options);
		void f_Output(EOutputType _OutputType = EOutputType_HumanReadable);
		void f_Output(NStr::CStr const &_OutputType);
		void f_Output(NEncoding::CEJSONSorted const &_Params);
		void f_ReverseRows();
		bool f_IsRounded() const;
		bool f_IsColor() const;

		static NEncoding::CEJSONOrdered::CKeyValue fs_OutputTypeOption(EOutputType _Default = EOutputType_HumanReadable);
		static EOutputType fs_ParseOutputTypeOption(NStr::CStr const &_String);
		static EOutputType fs_ParseOutputTypeOption(NEncoding::CEJSONSorted const &_Params);

	private:
		void fp_AddHeading(NStr::CStr const &_Heading);
		void fp_AddRowColumn(NContainer::TCVector<NContainer::TCVector<NStr::CStr>> &o_RowColumns, NStr::CStr const &_Value);
		void fp_Output(NStr::CStr const &_String) const;

		NFunction::TCFunction<void (NStr::CStr const &_Output)> mp_fOutput;

		NContainer::TCVector<NStr::CStr> mp_Headings;
		NContainer::TCVector<NContainer::TCVector<NContainer::TCVector<NStr::CStr>>> mp_Rows;
		NContainer::TCVector<zuint32> mp_Widths;
		NContainer::TCMap<uint32, uint32> mp_MaxWidths;
		NContainer::TCVector<zbool> mp_AlignRight;
		NContainer::TCSet<mint> mp_RowSeparators;
		NContainer::TCVector<NStr::CStr> mp_Description;
		CColumnHelper *mp_pColumnsHelper = nullptr;
		NStr::CStr mp_Prefix;
		int32 mp_DescriptionWidth = 0;
		uint32 mp_AvailableWidth = 0;
		EOption mp_Options = EOption_None;
		EAnsiEncodingFlag mp_AnsiFlags = EAnsiEncodingFlag_None;
	};
}

#ifndef DMibPNoShortCuts
	using namespace NMib::NCommandLine;
#endif

#include "Malterlib_CommandLine_TableRenderer.hpp"
