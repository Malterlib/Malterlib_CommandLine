// Copyright © 2019 Nonna Holding AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

namespace NMib::NCommandLine
{
	COneOf::COneOf(NEncoding::CEJSONOrdered const &_Config)
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
				Array.f_Insert(NEncoding::CEJSONOrdered(p_Config));
				return false;
			}
			()
			, ...
		);
	}

	COneOf::operator NEncoding::CEJSONOrdered () &&
	{
		return NEncoding::fg_UserTypeOrdered("$OneOf", m_Config.f_ToJson());
	}

	COneOf::operator NEncoding::CEJSONOrdered () const &
	{
		return NEncoding::fg_UserTypeOrdered("$OneOf", m_Config.f_ToJson());
	}

	COneOfType::COneOfType(NEncoding::CEJSONOrdered const &_Config)
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
				Array.f_Insert(NEncoding::CEJSONOrdered(p_Config));
				return false;
			}
			()
			, ...
		);
	}

	COneOfType::operator NEncoding::CEJSONOrdered () &&
	{
		return NEncoding::fg_UserTypeOrdered("$OneOfType", m_Config.f_ToJson());
	}

	COneOfType::operator NEncoding::CEJSONOrdered () const &
	{
		return NEncoding::fg_UserTypeOrdered("$OneOfType", m_Config.f_ToJson());
	}
}
