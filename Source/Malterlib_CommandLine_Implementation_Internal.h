// Copyright © 2019 Nonna Holding AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

#include "Malterlib_CommandLine_AnsiEncoding.h"
#include <Mib/Encoding/EJSON>

namespace NMib::NCommandLine
{
	template <typename t_CCustomization>
	struct TCCommandLineSpecification<t_CCustomization>::CInternal
	{
		enum EColor
		{
			EColor_None
			, EColor_Executable
			, EColor_Command
			, EColor_Number
			, EColor_String
			, EColor_Constant
			, EColor_Date
			, EColor_Binary
			, EColor_Parameter
			, EColor_GlobalOption
			, EColor_SectionOption
			, EColor_Option
			, EColor_Value
			, EColor_Error
			, EColor_Heading1
			, EColor_Default
		};

		struct CSection;
		struct CCommandCommon;

		struct CValue
		{
			CValue(NStr::CStr const &_Identifier, bool _bOptional)
				: m_Identifier(_Identifier)
				, m_bOptional(_bOptional)
			{
			}
			void f_Parse(NEncoding::CEJSON const &_Option);
			NEncoding::CEJSON fp_ParseEJSON(NStr::CStr const &_Value, NStr::CStr const &_Error) const;
			NEncoding::CEJSON fp_ConvertValue(NEncoding::CEJSON const &_Template, NEncoding::CEJSON const &_Value, NStr::CStr const &_Identifier, bool _bStrict, NCommandLine::EAnsiEncodingFlag _AnsiFlags) const;
			NStr::CStr fp_FormatValue(NEncoding::CEJSON const &_Template, NEncoding::CEJSON const &_Value, NStr::CStr const &_Identifier, NCommandLine::EAnsiEncodingFlag _AnsiFlags) const;
			void fp_ValidateTemplate(NEncoding::CEJSON const &_Template, NStr::CStr const &_Identifier, bool _bPrevIsSetOf) const;
			NEncoding::CEJSON f_ConvertValue(NEncoding::CEJSON const &_Value, EColor _Color, NCommandLine::EAnsiEncodingFlag _AnsiFlags) const;
			NEncoding::CEJSON f_ConvertValue(NEncoding::CEJSON const &_Value, NStr::CStr const &_Identifier, EColor _Color, NCommandLine::EAnsiEncodingFlag _AnsiFlags) const;
			void f_AppendConvertValue(NEncoding::CEJSON &o_Value, NEncoding::CEJSON const &_Value, EColor _Color, NCommandLine::EAnsiEncodingFlag _AnsiFlags) const;
			void f_AppendConvertValue(NEncoding::CEJSON &o_Value, NEncoding::CEJSON const &_Value, NStr::CStr const &_Identifier, EColor _Color, NCommandLine::EAnsiEncodingFlag _AnsiFlags) const;
			NStr::CStr f_FormatValue(NEncoding::CEJSON const &_Value, NCommandLine::EAnsiEncodingFlag _AnsiFlags) const;

			NStr::CStr const m_Identifier;
			bool const m_bOptional;

			NEncoding::CEJSON m_TypeTemplate;
			NEncoding::CEJSON m_Default;

			NStr::CStr m_ShortDescription;
			NStr::CStr m_LongDescription;
		};

		struct COptionSet
		{
			NContainer::TCSet<NStr::CStr> m_Allowed;
			NContainer::TCSet<NStr::CStr> m_Disallowed;
			bool m_bAllowedSpecified = false;
		};

		struct COption : public CValue
		{
			COption(NStr::CStr const &_Identifier, NContainer::TCVector<NStr::CStr> const &_Names, CCommandCommon *_pCommand, bool _bOptional)
				: CValue(_Identifier, _bOptional)
				, m_pCommand(_pCommand)
				, m_Names(_Names)
			{
			}
			void f_ParseOption(NEncoding::CEJSON const &_Option);
			bool f_IsEnabled(COptionSet const &_OptionSet, bool _bIsDirect) const;

			NContainer::TCVector<NStr::CStr> const m_Names;
			CCommandCommon * const m_pCommand = nullptr;
			bool m_bDefaultEnabled = true;
			bool m_bHidden = false;
			bool m_bCanNegate = true;
			bool m_bDisablesAllErrors = false;
			bool m_bValidForDirectCommand = true;
		};

		struct CParameter : public CValue
		{
			CParameter(NStr::CStr const &_Identifier, bool _bOptional, bool _bVector)
				: CValue(_Identifier, _bOptional)
				, m_bVector(_bVector)
			{
			}
			void f_ParseParameter(NEncoding::CEJSON const &_Parameter);

			bool m_bVector;
		};

		struct CCommandCommon
		{
			CCommandCommon(CSection *_pSection, NContainer::TCVector<NStr::CStr> const &_Names)
				: m_pSection(_pSection)
				, m_Names(_Names)
			{
			}
			void f_RegisterOptions(CInternal &_Internal, NEncoding::CEJSON const &_Options);

			CSection * const m_pSection;
			NContainer::TCVector<NStr::CStr> const m_Names;

			NStr::CStr m_Category;

			NStr::CStr m_ShortDescription;
			NStr::CStr m_LongDescription;
			NStr::CStr m_OutputDescription;
			NContainer::TCMap<NStr::CStr, NStr::CStr> m_StatusDescription;

			NContainer::TCLinkedList<CParameter> m_Parameters;
			NContainer::TCMap<NStr::CStr, CParameter *> m_ParametersByIdentifier;

			NContainer::TCLinkedList<COption> m_Options;
			NContainer::TCMap<NStr::CStr, COption *> m_OptionsByIdentifier;
			NContainer::TCMap<NStr::CStr, COption *> m_OptionsByName;

			COptionSet m_SectionOptionSet;
			COptionSet m_GlobalOptionSet;

			NStorage::TCSharedPointer
				<
					NFunction::TCFunctionMovable<uint32 (NEncoding::CEJSON const &_Parameters, CCommandLineClient &_CommandLineClient)>
				>
				m_pDirectRunCommand
			;

			bool m_bErrorOnCommandAsParameter = true;
			bool m_bErrorOnOptionAsParameter = true;
			bool m_bErrorOnOptionAsParameterWhenDefaultCommand = true;
			bool m_bAlwaysVerbose = false;
			bool m_bGreedyDefaultCommand = false;
			bool m_bGreedyDefaultCommandParameters = false;
			bool m_bShowOptionsInCommandEntry = false;
			bool m_bShowParametersStart = true;
		};

		using CCommand = typename t_CCustomization::template TCInternalCommand<CCommandCommon>::CCommand;

		struct CSection
		{
			NStr::CStr m_Heading;
			NStr::CStr m_Description;
			NContainer::TCLinkedList<CCommand> m_Commands;

			NContainer::TCMap<NStr::CStr, NContainer::TCVector<NStr::CStr>> m_CommandOptionsIdentifiers;
			NContainer::TCMap<NStr::CStr, NContainer::TCVector<NStr::CStr>> m_CommandParameterIdentifiers;
			NContainer::TCMap<NStr::CStr, NContainer::TCVector<NStr::CStr>> m_CommandOptionsNames;

			NContainer::TCLinkedList<COption> m_SectionOptions;
			NContainer::TCMap<NStr::CStr, COption *> m_SectionOptionsByIdentifier;
			NContainer::TCMap<NStr::CStr, COption *> m_SectionOptionsByName;
		};

		CCommand *f_RegisterCommand(CSection &_Section, NEncoding::CEJSON const &_CommandDescription);
		NEncoding::CEJSON f_ValidateParams(CCommand const &_Command, NEncoding::CEJSON const &_Params) const;
		void f_CheckName(NStr::CStr const &_Name);
		static void fs_CheckValidObject(NEncoding::CEJSON const &_ToCheck, NContainer::TCSet<NStr::CStr> const &_AllowedKeys);
		static NStr::CStr fs_Color(NStr::CStr const &_String, EColor _Color, NCommandLine::EAnsiEncodingFlag _AnsiFlags);

		NContainer::TCLinkedList<CSection> m_Sections;
		NContainer::TCLinkedList<COption> m_GlobalOptions;

		NContainer::TCMap<NStr::CStr, CCommand *> m_CommandByName;

		NContainer::TCMap<NStr::CStr, NContainer::TCVector<NStr::CStr>> m_CommandOptionsIdentifiers;
		NContainer::TCMap<NStr::CStr, NContainer::TCVector<NStr::CStr>> m_CommandParameterIdentifiers;
		NContainer::TCMap<NStr::CStr, NContainer::TCVector<NStr::CStr>> m_CommandOptionsNames;

		NContainer::TCMap<NStr::CStr, NContainer::TCVector<NStr::CStr>> m_SectionOptionsIdentifiers;
		NContainer::TCMap<NStr::CStr, NContainer::TCVector<NStr::CStr>> m_SectionOptionsNames;

		NContainer::TCMap<NStr::CStr, COption *> m_GlobalOptionsByIdentifier;
		NContainer::TCMap<NStr::CStr, COption *> m_GlobalOptionsByName;

		CCommand *m_pDefaultCommand = nullptr;
	};
}
