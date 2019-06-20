// Copyright © 2018 Nonna Holding AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

#include <Mib/Function/Function>

namespace NMib::NCommandLine
{
	struct CTableRenderHelper
	{
		enum EOption : uint32
		{
			EOption_None = 0
			, EOption_Rounded = DMibBit(0)
		};

		CTableRenderHelper(NFunction::TCFunction<void (NStr::CStr const &_Output)> const &_fOutput, EOption _Options, EAnsiEncodingFlag _AnsiFlags);
		CTableRenderHelper(CTableRenderHelper &&) = default;

		void f_AddDescription(NStr::CStr const &_Description);
		template <typename ...tfp_CString>
		void f_AddHeadings(tfp_CString &&...p_Headings);
		template <typename ...tfp_CString>
		void f_AddRow(tfp_CString &&...p_RowColumns);
		void f_Output() const;
		bool f_IsRounded() const;
		bool f_IsColor() const;

	private:
		void fp_AddHeading(NStr::CStr const &_Heading);
		void fp_AddRowColumn(NContainer::TCVector<NContainer::TCVector<NStr::CStr>> &o_RowColumns, NStr::CStr const &_Value);
		void fp_Output(NStr::CStr const &_String) const;

		NFunction::TCFunction<void (NStr::CStr const &_Output)> mp_fOutput;

		NContainer::TCVector<NStr::CStr> mp_Headings;
		NContainer::TCVector<NContainer::TCVector<NContainer::TCVector<NStr::CStr>>> mp_Rows;
		NContainer::TCVector<zint32> mp_MaxWidths;
		NContainer::TCVector<NStr::CStr> mp_Description;
		int32 mp_DescriptionWidth = 0;
		EOption mp_Options = EOption_None;
		EAnsiEncodingFlag mp_AnsiFlags = EAnsiEncodingFlag_None;
	};
}

#ifndef DMibPNoShortCuts
	using namespace NMib::NCommandLine;
#endif

#include "Malterlib_CommandLine_TableRenderer.hpp"
