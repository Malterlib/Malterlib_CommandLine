// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

namespace NMib::NCommandLine
{
	COneOf::COneOf(NEncoding::CEJsonOrdered const &_Config)
	{
		m_Config.f_Array().f_Insert(_Config);
	}

	template <typename ...tfp_CParams>
	COneOf::COneOf(tfp_CParams const &...p_Config)
	{
		auto &Array = m_Config.f_Array();

		(
			[&]
			{
				Array.f_Insert(NEncoding::CEJsonOrdered(p_Config));
				return false;
			}
			()
			, ...
		);
	}

	COneOf::operator NEncoding::CEJsonOrdered () &&
	{
		return NEncoding::fg_UserTypeOrdered("$OneOf", m_Config.f_ToJson());
	}

	COneOf::operator NEncoding::CEJsonOrdered () const &
	{
		return NEncoding::fg_UserTypeOrdered("$OneOf", m_Config.f_ToJson());
	}

	COneOfType::COneOfType(NEncoding::CEJsonOrdered const &_Config)
	{
		m_Config.f_Array().f_Insert(_Config);
	}

	template <typename ...tfp_CParams>
	COneOfType::COneOfType(tfp_CParams const &...p_Config)
	{
		auto &Array = m_Config.f_Array();

		(
			[&]
			{
				Array.f_Insert(NEncoding::CEJsonOrdered(p_Config));
				return false;
			}
			()
			, ...
		);
	}

	COneOfType::operator NEncoding::CEJsonOrdered () &&
	{
		return NEncoding::fg_UserTypeOrdered("$OneOfType", m_Config.f_ToJson());
	}

	COneOfType::operator NEncoding::CEJsonOrdered () const &
	{
		return NEncoding::fg_UserTypeOrdered("$OneOfType", m_Config.f_ToJson());
	}
}
