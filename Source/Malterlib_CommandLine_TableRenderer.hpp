// Copyright © 2018 Nonna Holding AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

namespace NMib::NCommandLine
{
	template <typename ...tfp_CString>
	void CTableRenderHelper::f_AddHeadings(tfp_CString &&...p_Headings)
		requires (sizeof...(p_Headings) >= 1)
	{
		DMibRequire(mp_Headings.f_IsEmpty());

		TCInitializerList<bool> Dummy =
			{
				[&]
				{
					fp_AddHeading(p_Headings);
					return true;
				}()...
			}
		;
		(void)Dummy;
	}

	template <typename ...tfp_CString>
	void CTableRenderHelper::f_AddRow(tfp_CString &&...p_RowColumns)
		requires (sizeof...(p_RowColumns) >= 1)
	{
		DMibRequire(!mp_Widths.f_IsEmpty());
		DMibRequire(sizeof...(p_RowColumns) == mp_Headings.f_GetLen());

		NContainer::TCVector<NContainer::TCVector<NStr::CStr>> RowColumns;
		TCInitializerList<bool> Dummy =
			{
				[&]
				{
					fp_AddRowColumn(RowColumns, NStr::CStr::fs_ToStr(p_RowColumns));
					return true;
				}()...
			}
		;

		mp_Rows.f_Insert(RowColumns);
		(void)Dummy;
	}
}
