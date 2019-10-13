// Copyright © 2019 Nonna Holding AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

namespace NMib::NCommandLine
{
	[[maybe_unused]] static NStr::CStr fg_ParseIdentifier(NStr::CStr const &_Name, bool &o_bIsOptional, bool &o_bIsVector)
	{
		if (_Name.f_IsEmpty())
			DMibError("You cannot specify an empty identifier. If you want to match any key use '*' as identifier.");

		if (_Name == "*")
		{
			o_bIsOptional = false;
			o_bIsVector = false;
			return _Name;
		}

		NStr::CStr Identifier = _Name;
		o_bIsOptional = false;
		if (Identifier[Identifier.f_GetLen() - 1] == '?')
		{
			o_bIsOptional = true;
			Identifier = Identifier.f_Left(Identifier.f_GetLen() - 1);
		}

		o_bIsVector = false;
		if (Identifier.f_GetLen() >= 3 && Identifier.f_FindReverse("...") == Identifier.f_GetLen() - 3)
		{
			o_bIsVector = true;
			Identifier = Identifier.f_Left(Identifier.f_GetLen() - 3);
		}

		return Identifier;
	}

	[[maybe_unused]] static void fg_ParseDescription(NEncoding::CEJSON const &_Object, NStr::CStr &o_Short, NStr::CStr &o_Long)
	{
		NStr::CStr Description = _Object["Description"].f_String();
		o_Short = fg_GetStrLineSep(Description).f_Trim();
		o_Long = Description.f_Trim();
	}

	template <typename t_CCustomization>
	NStr::CStr TCCommandLineSpecification<t_CCustomization>::CInternal::fs_Color(NStr::CStr const &_String, EColor _Color, NCommandLine::EAnsiEncodingFlag _AnsiFlags)
	{
		using namespace NStr;
		if (!(_AnsiFlags & EAnsiEncodingFlag_Color))
			return _String;

		CAnsiEncoding AnsiEncoding(_AnsiFlags);

		switch (_Color)
		{
		case EColor_Executable: return "{}{}{}"_f << AnsiEncoding.f_ForegroundRGB(0x00, 0xe4, 0xe6) << _String << AnsiEncoding.f_Default();
		case EColor_Command: return "{}{}{}"_f << AnsiEncoding.f_ForegroundRGB(0x1c, 0xb9, 0x00) << _String << AnsiEncoding.f_Default();
		case EColor_Parameter: return "{}{}{}"_f << AnsiEncoding.f_ForegroundRGB(0xb8, 0xaa, 0xff) << _String << AnsiEncoding.f_Default();
		case EColor_GlobalOption: return "{}{}{}"_f << AnsiEncoding.f_ForegroundRGB(0xff, 0x77, 0x00) << _String << AnsiEncoding.f_Default();
		case EColor_SectionOption: return "{}{}{}"_f << AnsiEncoding.f_ForegroundRGB(0xff, 0xa6, 0x00) << _String << AnsiEncoding.f_Default();
		case EColor_Option: return "{}{}{}"_f << AnsiEncoding.f_ForegroundRGB(0xff, 0xd7, 0x00) << _String << AnsiEncoding.f_Default();
		case EColor_String: return "{}{}{}"_f << AnsiEncoding.f_ForegroundRGB(0x00, 0x9e, 0xff) << _String << AnsiEncoding.f_Default();
		case EColor_Number: return "{}{}{}"_f << AnsiEncoding.f_ForegroundRGB(0xff, 0x00, 0x80) << _String << AnsiEncoding.f_Default();
		case EColor_Constant: return "{}{}{}"_f << AnsiEncoding.f_ForegroundRGB(0xff, 0x8a, 0xc5) << _String << AnsiEncoding.f_Default();
		case EColor_Date: return "{}{}{}"_f << AnsiEncoding.f_ForegroundRGB(0xff, 0x5b, 0xad) << _String << AnsiEncoding.f_Default();
		case EColor_Binary: return "{}{}{}"_f << AnsiEncoding.f_ForegroundRGB(0x80, 0x80, 0x80) << _String << AnsiEncoding.f_Default();
		case EColor_Value: return "{}{}{}"_f << AnsiEncoding.f_ForegroundRGB(0xc0, 0xc0, 0xc0) << _String << AnsiEncoding.f_Default();
		case EColor_Error: return "{}{}{}"_f << AnsiEncoding.f_ForegroundRGB(0xff, 0x3f, 0x1c) << _String << AnsiEncoding.f_Default();
		case EColor_Heading1: return "{}{}{}"_f << AnsiEncoding.f_Bold() << _String << AnsiEncoding.f_Default();
		case EColor_Default: return "{}{}{}"_f << AnsiEncoding.f_ForegroundRGB(0x90, 0x90, 0x90) << _String << AnsiEncoding.f_Default();

		case EColor_None:
		default: return _String;
		}
	}

	template <typename t_CCustomization>
	void TCCommandLineSpecification<t_CCustomization>::CInternal::CValue::f_Parse(NEncoding::CEJSON const &_Value)
	{
		if (auto *pDefault = _Value.f_GetMember("Default"))
			m_Default = *pDefault;
		if (auto *pType = _Value.f_GetMember("Type"))
			m_TypeTemplate = *pType;

		if (!m_Default.f_IsValid() && !m_TypeTemplate.f_IsValid())
			DMibError("An option needs to specify at least 'Default' or 'Type'");
		else if (m_Default.f_IsValid() && !m_TypeTemplate.f_IsValid())
			m_TypeTemplate = m_Default;

		fp_ValidateTemplate(m_TypeTemplate, m_Identifier, false);

		if (m_Default.f_IsValid())
			m_Default = f_ConvertValue(m_Default, EColor_None, NCommandLine::EAnsiEncodingFlag_None); // This will do checking to make sure that the default value matches the template

		fg_ParseDescription(_Value, m_ShortDescription, m_LongDescription);
	}

	template <typename t_CCustomization>
	NEncoding::CEJSON TCCommandLineSpecification<t_CCustomization>::CInternal::f_ValidateParams(CCommand const &_Command, NEncoding::CEJSON const &_Params) const
	{
		NEncoding::CEJSON Params;

		auto fCheckValue = [&](CValue const &_Value)
			{
				if (!_Params.f_GetMember(_Value.m_Identifier))
				{
					if (!_Value.m_bOptional || _Value.m_Default.f_IsValid())
						DMibError(fg_Format("Missing non-optional parameter: {}", _Value.m_Identifier));
				}
			}
		;

		for (auto &Option : m_GlobalOptions)
			fCheckValue(Option);
		for (auto &Option : _Command.m_pSection->m_SectionOptions)
			fCheckValue(Option);
		for (auto &Option : _Command.m_Options)
			fCheckValue(Option);
		for (auto &Parameter : _Command.m_Parameters)
			fCheckValue(Parameter);

		bool bFoundCommand = false;
		for (auto &Param : _Params.f_Object())
		{
			CValue *pValue = nullptr;
			NStr::CStr const &ParamName = Param.f_Name();
			if (ParamName == "Command")
			{
				if (!Param.f_Value().f_IsString())
					DMibError("'Command' needs to be string");
				Params[ParamName] = Param.f_Value().f_String();
				bFoundCommand = true;
			}
			else
			{
				if (auto pParameter = _Command.m_ParametersByIdentifier.f_FindEqual(ParamName))
					pValue = *pParameter;
				else if (auto pOption = _Command.m_OptionsByIdentifier.f_FindEqual(ParamName))
					pValue = *pOption;
				else if (auto pOption = _Command.m_pSection->m_SectionOptionsByIdentifier.f_FindEqual(ParamName))
					pValue = *pOption;
				else if (auto pOption = m_GlobalOptionsByIdentifier.f_FindEqual(ParamName))
					pValue = *pOption;
				if (!pValue)
					DMibError(fg_Format("Unexpected parameter '{}' in command params", ParamName));
				Params[ParamName] = pValue->f_ConvertValue(Param.f_Value(), EColor_None, NCommandLine::EAnsiEncodingFlag_None);
			}
		}

		if (!bFoundCommand)
			DMibError("'Command' missing in params");

		return Params;
	}

	template <typename t_CCustomization>
	NEncoding::CEJSON TCCommandLineSpecification<t_CCustomization>::CInternal::CValue::f_ConvertValue(NEncoding::CEJSON const &_Value, EColor _Color, NCommandLine::EAnsiEncodingFlag _AnsiFlags) const
	{
		return fp_ConvertValue(m_TypeTemplate, _Value, fs_Color(m_Identifier, _Color, _AnsiFlags), false, _AnsiFlags);
	}

	template <typename t_CCustomization>
	NEncoding::CEJSON TCCommandLineSpecification<t_CCustomization>::CInternal::CValue::f_ConvertValue
		(
		 	NEncoding::CEJSON const &_Value
		 	, NStr::CStr const &_Identifier
		 	, EColor _Color
		 	, NCommandLine::EAnsiEncodingFlag _AnsiFlags
		) const
	{
		return fp_ConvertValue(m_TypeTemplate, _Value, fs_Color(_Identifier, _Color, _AnsiFlags), false, _AnsiFlags);
	}

	template <typename t_CCustomization>
	NStr::CStr TCCommandLineSpecification<t_CCustomization>::CInternal::CValue::f_FormatValue(NEncoding::CEJSON const &_Value, NCommandLine::EAnsiEncodingFlag _AnsiFlags) const
	{
		return fp_FormatValue(m_TypeTemplate, _Value, m_Identifier, _AnsiFlags);
	}

	template <typename t_CCustomization>
	void TCCommandLineSpecification<t_CCustomization>::CInternal::CValue::f_AppendConvertValue
		(
		 	NEncoding::CEJSON &o_Value
		 	, NEncoding::CEJSON const &_Value
		 	, EColor _Color
		 	, NCommandLine::EAnsiEncodingFlag _AnsiFlags
		) const
	{
		NEncoding::CEJSON NewParams = fp_ConvertValue(m_TypeTemplate.f_Array()[0], _Value, fs_Color(m_Identifier, _Color, _AnsiFlags), false, _AnsiFlags);
		o_Value.f_Array().f_Insert(NewParams);
	}

	template <typename t_CCustomization>
	void TCCommandLineSpecification<t_CCustomization>::CInternal::CValue::f_AppendConvertValue
		(
		 	NEncoding::CEJSON &o_Value
		 	, NEncoding::CEJSON const &_Value
		 	, NStr::CStr const &_Identifier
		 	, EColor _Color
		 	, NCommandLine::EAnsiEncodingFlag _AnsiFlags
		) const
	{
		NEncoding::CEJSON NewParams = fp_ConvertValue(m_TypeTemplate.f_Array()[0], _Value, fs_Color(_Identifier, _Color, _AnsiFlags), false, _AnsiFlags);
		o_Value.f_Array().f_Insert(NewParams);
	}

	template <typename t_CCustomization>
	NEncoding::CEJSON TCCommandLineSpecification<t_CCustomization>::CInternal::CValue::fp_ParseEJSON(NStr::CStr const &_Value, NStr::CStr const &_Error) const
	{
		try
		{
			return NEncoding::CEJSON::fs_FromString(_Value);
		}
		catch (NException::CException const &_Exception)
		{
			DMibError(fg_Format("{}: {}", _Error, _Exception.f_GetErrorStr()));
		}
	}

	template <typename t_CCustomization>
	void TCCommandLineSpecification<t_CCustomization>::CInternal::CValue::fp_ValidateTemplate(NEncoding::CEJSON const &_Template, NStr::CStr const &_Identifier, bool _bPrevIsSetOf) const
	{
		using namespace NEncoding;
		switch (_Template.f_EType())
		{
		case EEJSONType_String:
		case EEJSONType_Integer:
		case EEJSONType_Float:
		case EEJSONType_Boolean:
		case EEJSONType_Binary:
		case EEJSONType_Date:
			break;
		case EEJSONType_UserType:
			{
				auto &TemplateUserType = _Template.f_UserType();
				if (TemplateUserType.m_Type == "$OneOf" || TemplateUserType.m_Type == "$OneOfType")
				{
					bool bIsType = TemplateUserType.m_Type == "$OneOfType";
					NEncoding::CEJSON TemplateSet = NEncoding::CEJSON::fs_FromJSON(TemplateUserType.m_Value);

					auto &Set = TemplateSet.f_Array();

					NStr::CStr::CFormat FormatIdentifier("{}.{{{}}");
					if (bIsType)
						FormatIdentifier.f_SetFormatStr("{}.({})");

					mint iSet = 0;
					FormatIdentifier << _Identifier << iSet;

					for (auto &PossibleMatch : Set)
					{
						fp_ValidateTemplate(PossibleMatch, FormatIdentifier, !bIsType);
						++iSet;
					}
				}
			}
			break;
		case EEJSONType_Object:
			{
				auto &TemplateObject = _Template.f_Object();

				NContainer::TCSet<NStr::CStr> TemplateMemberNames;

				for (auto &TemplateMember : TemplateObject)
				{
					bool bOptional = false;
					bool bVector = false;
					NStr::CStr Identifier = fg_ParseIdentifier(TemplateMember.f_Name(), bOptional, bVector);

					if (bVector)
						DMibError("... vector form is not supported supported for object members");

					if (!TemplateMemberNames(Identifier).f_WasCreated())
						DMibError(fg_Format("[{}] Duplicate member '{}' in template object", _Identifier, Identifier));

					fp_ValidateTemplate(TemplateMember.f_Value(), fg_Format("{}.{}", _Identifier, Identifier), false);
				}
			}
			break;
		case EEJSONType_Array:
			{
				auto &TemplateArray = _Template.f_Array();
				if (TemplateArray.f_IsEmpty())
					break;

				if (TemplateArray.f_GetLen() > 1)
					DMibError(fg_Format("[{}] Template array can only contain one value. Use COneOf or COneOfType to specify different options. Array: {}", _Identifier, TemplateArray));

				auto &Template = TemplateArray.f_GetFirst();
				fp_ValidateTemplate(Template, fg_Format("{}.[]", _Identifier), false);
			}
			break;
		case EEJSONType_Null:
			if (_bPrevIsSetOf)
				break;
			DMibError("null can only be used in COneOf statements");
			break;
		default:
			DMibError("Invalid template type");
			break;
		}
	}

	DMibImpErrorClassDefine(CCommandLineConvertException, NMib::NException::CException);

#	define DMibCommandLineConvertException(_Description) DMibImpError(CCommandLineConvertException, _Description)

	static NStr::CStr fg_GetEJSONTypeName(NEncoding::EEJSONType _Type)
	{
		using namespace NEncoding;
		switch (_Type)
		{
		case EEJSONType_Null: return "null";
		case EEJSONType_String: return "string";
		case EEJSONType_Integer: return "integer";
		case EEJSONType_Float: return "float";
		case EEJSONType_Boolean: return "boolean";
		case EEJSONType_Object: return "object";
		case EEJSONType_Array: return "array";
		case EEJSONType_Date: return "date";
		case EEJSONType_Binary: return "binary";
		case EEJSONType_UserType: return "user type";
		default: DMibNeverGetHere; return "unknown";
		}
	}

	static void fg_CheckType(NEncoding::CEJSON const &_Value, NEncoding::EEJSONType _Type)
	{
		if (_Value.f_EType() != _Type)
			DMibError(fg_Format("Expected '{}' but got '{}': {}", fg_GetEJSONTypeName(_Type), fg_GetEJSONTypeName(_Value.f_EType()), _Value).f_TrimRight());
	}

	template <typename t_CCustomization>
	NEncoding::CEJSON TCCommandLineSpecification<t_CCustomization>::CInternal::CValue::fp_ConvertValue
		(
			NEncoding::CEJSON const &_Template
			, NEncoding::CEJSON const &_Value
			, NStr::CStr const &_Identifier
			, bool _bStrict
			, NCommandLine::EAnsiEncodingFlag _AnsiFlags
		) const
	{
		using namespace NEncoding;
		using namespace NStr;
		CEJSON Return;
		try
		{
			switch (_Template.f_EType())
			{
			case EEJSONType_String:
				{
					fg_CheckType(_Value, _Template.f_EType());
					Return = _Value.f_String();
				}
				break;
			case EEJSONType_Integer:
				{
					if (_bStrict)
					{
						fg_CheckType(_Value, _Template.f_EType());
						Return = _Value.f_Integer();
					}
					else if (_Value.f_IsString())
					{
						int64 Value = _Value.f_String().f_ToIntExact(TCLimitsInt<int64>::mc_Max);
						if (Value == TCLimitsInt<int64>::mc_Max)
						{
							Value = _Value.f_String().f_ToIntExact(TCLimitsInt<int64>::mc_Min);
							if (Value == TCLimitsInt<int64>::mc_Min)
								DMibError(fg_Format("Failed to parse {} as a integer value", fs_Color("\"{}\""_f << _Value.f_String(), EColor_String, _AnsiFlags)));
						}
						Return = Value;
					}
					else
						Return = _Value.f_AsInteger();
				}
				break;
			case EEJSONType_Float:
				{
					if (_bStrict)
					{
						fg_CheckType(_Value, _Template.f_EType());
						Return = _Value.f_Float();
					}
					else if (_Value.f_IsString())
					{
						fp64 Value = _Value.f_String().f_ToFloatExact(fp64::fs_Inf());
						if (Value == fp64::fs_Inf())
							DMibError(fg_Format("Failed to parse {} as a float value", fs_Color("\"{}\""_f << _Value.f_String(), EColor_String, _AnsiFlags)));
						Return = Value;
					}
					else
						Return = _Value.f_AsFloat();
				}
				break;
			case EEJSONType_Boolean:
				{
					if (_bStrict)
					{
						fg_CheckType(_Value, _Template.f_EType());
						Return = _Value.f_Boolean();
					}
					else if (_Value.f_IsString())
					{
						if (_Value.f_String() == "true")
							Return = true;
						else if (_Value.f_String() == "false")
							Return = false;
						else
						{
							int64 Value = _Value.f_String().f_ToIntExact(TCLimitsInt<int64>::mc_Max);
							if (Value == 1)
								Return = true;
							else if (Value == 0)
								Return = false;
							else
								DMibError(fg_Format("Failed to parse {} as a boolean value", fs_Color("\"{}\""_f << _Value.f_String(), EColor_String, _AnsiFlags)));
						}
					}
					else
						Return = _Value.f_AsBoolean();
				}
				break;
			case EEJSONType_Binary:
				{
					if (_Value.f_IsBinary())
						Return = _Value;
					else if (_Value.f_IsString())
					{
						NContainer::CByteVector Data;
						fg_Base64Decode(_Value.f_String(), Data);
						Return = Data;
					}
					else
						DMibError("Binary can only be converted from string");
				}
				break;
			case EEJSONType_Date:
				{
					if (_Value.f_IsDate())
					{
						Return = _Value;
						break;
					}
					if (!_Value.f_IsString())
						DMibError("Date can only be converted from string");

					auto fReportError = [&](CStr const &_Error)
						{
							DMibError
								(
									fg_Format
									(
										"Failed to parse {} as a date: {}. Date format is: Year-Month-Day [Hour[:Minute[:Second[.Fraction]]]]"
										, fs_Color("\"{}\""_f << _Value.f_String(), EColor_String, _AnsiFlags)
										, _Error
									)
								)
							;
						}
					;

					auto fParseInt = [&](ch8 const *&_pParse, auto _Type, ch8 const *_pTerminators)
						{
							bool bFailed = false;
							auto Return = fg_StrToIntParse(_pParse, _Type, _pTerminators, false, EStrToIntParseMode_Base10, &bFailed);
							if (bFailed)
								fReportError(fg_Format("Failed to parse {} as a integer", fs_Color("\"{}\""_f << _pParse, EColor_String, _AnsiFlags)));
							if (*_pParse == _pTerminators[0])
								++_pParse;
							return Return;
						}
					;

					if (_Value.f_String() == "INVALID")
					{
						Return = NTime::CTime();
						break;
					}

					ch8 const *pParse = _Value.f_String();

					fg_ParseWhiteSpace(pParse);
					if (!*pParse)
						fReportError("Missing year");
					int64 Year = fParseInt(pParse, int64(), "-");
					if (!*pParse)
						fReportError("Missing month");
					uint32 Month = fParseInt(pParse, uint32(), "-");
					if (!*pParse)
						fReportError("Missing day");
					uint32 Day = fParseInt(pParse, uint32(), " ");
					uint32 Hour = 0;
					uint32 Minute = 0;
					uint32 Second = 0;
					fp64 Fraction = 0;
					if (*pParse)
						Hour = fParseInt(pParse, uint32(), ":");
					if (*pParse)
						Minute = fParseInt(pParse, uint32(), ":");
					if (*pParse)
						Second = fParseInt(pParse, uint32(), ".");
					if (*pParse)
					{
						--pParse;
						Fraction = fg_StrToFloatParse(pParse, fp64::fs_Inf(), (ch8 const *)nullptr, false, (ch8 const *)nullptr);
						if (Fraction == fp64::fs_Inf())
							fReportError(fg_Format("Failed to parse {} as a float", fs_Color("\"{}\""_f << pParse, EColor_String, _AnsiFlags)));
					}

					if (*pParse)
						fReportError(fg_Format("Unexpected: {}", pParse));

					if (fg_Clamp(Month, 1u, 12u) != Month)
						fReportError("Invalid month");
					if (fg_Clamp(Day, 1u, mint(NTime::CTimeConvert::fs_GetDaysInMonth(Year, Month - 1))) != Day)
						fReportError("Invalid day");
					if (fg_Clamp(Hour, 0u, 23u) != Hour)
						fReportError("Invalid hour");
					if (fg_Clamp(Minute, 0u, 59u) != Minute)
						fReportError("Invalid minute");
					if (fg_Clamp(Second, 0u, 59u) != Second)
						fReportError("Invalid second");
					if (fg_Clamp(Fraction, 0.0, 1.0) != Fraction)
						fReportError("Invalid fraction");

					Return = NTime::CTimeConvert::fs_CreateTime(Year, Month, Day, Hour, Minute, Second, Fraction);
				}
				break;
			case EEJSONType_UserType:
				{
					auto &TemplateUserType = _Template.f_UserType();
					if (TemplateUserType.m_Type == "$OneOf" || TemplateUserType.m_Type == "$OneOfType")
					{
						bool bIsType = TemplateUserType.m_Type == "$OneOfType";
						CEJSON TemplateSet = CEJSON::fs_FromJSON(TemplateUserType.m_Value);

						auto &Set = TemplateSet.f_Array();

						CStr::CFormat FormatIdentifier("{}.{{{}}");
						if (bIsType)
							FormatIdentifier.f_SetFormatStr("{}.({})");

						mint iSet = 0;
						FormatIdentifier << _Identifier << iSet;

						{
							NException::CDisableExceptionTraceScope DisableExceptionTrace;
							for (auto &PossibleMatch : Set)
							{
								try
								{
									CEJSON Value = fp_ConvertValue(PossibleMatch, _Value, FormatIdentifier, _bStrict, _AnsiFlags);
									if (bIsType)
										return Value;
									else if (Value == PossibleMatch)
										return Value;
								}
								catch (NException::CException const &)
								{
								}
								++iSet;
							}
						}

						DMibError(fg_Format("Could not match '{jp}' to any member in set: {}", _Value, TemplateSet.f_ToString(nullptr)));

						break;
					}
					if (_Value.f_IsUserType())
					{
						auto &UserType = _Value.f_UserType();
						if (UserType.m_Type != TemplateUserType.m_Type)
							DMibError(fg_Format("Incorrect user type: '{}' != '{}'", UserType.m_Type, TemplateUserType.m_Type));
						Return = _Value;
						break;
					}
					DMibError("Converting to user type is not supported");
				}
				break;
			case EEJSONType_Object:
				{
					CEJSON RawValue;
					if (_bStrict)
					{
						fg_CheckType(_Value, _Template.f_EType());
						RawValue = _Value.f_Object();
					}
					else if (_Value.f_IsObject())
						RawValue = _Value;
					else if (_Value.f_IsString())
					{
						RawValue = fp_ParseEJSON(_Value.f_String(), "Error parsing object parameter");
						if (!RawValue.f_IsObject())
							DMibError("Expected object");
					}
					else
						DMibError("Object can only be converted from string");

					auto &TemplateObject = _Template.f_Object();

					if (!TemplateObject.f_OrderedIterator())
					{
						// Allow anything
						Return = RawValue;
					}
					else
					{
						auto &InputObject = RawValue.f_Object();
						auto &OutputObject = Return.f_Object();
						NContainer::TCSet<CStr> TemplateMemberNames;

						CEJSON const *pWildcard = nullptr;

						for (auto &TemplateMember : TemplateObject)
						{
							bool bOptional = false;
							bool bVector = false;
							CStr Identifier = fg_ParseIdentifier(TemplateMember.f_Name(), bOptional, bVector);

							DMibCheck(!bVector);

							if (Identifier == "*")
							{
								DMibCheck(!pWildcard);
								pWildcard = &TemplateMember.f_Value();
								continue;
							}

							TemplateMemberNames[Identifier];

							auto *pInputMember = InputObject.f_GetMember(Identifier);
							if (!pInputMember)
							{
								if (!bOptional)
									DMibError(fg_Format("Missing required member '{}' in object", TemplateMember.f_Name()));
								continue;
							}

							OutputObject.f_CreateMember(Identifier)
								= fp_ConvertValue(TemplateMember.f_Value(), *pInputMember, fg_Format("{}.{}", _Identifier, Identifier), true, _AnsiFlags)
							;
						}

						for (auto &InputMember : InputObject)
						{
							if (!TemplateMemberNames.f_FindEqual(InputMember.f_Name()))
							{
								if (!pWildcard)
									DMibError(fg_Format("Unexpected member '{}' in input object", InputMember.f_Name()));
								OutputObject.f_CreateMember(InputMember.f_Name())
									= fp_ConvertValue(*pWildcard, InputMember.f_Value(), fg_Format("{}.{}", _Identifier, InputMember.f_Name()), true, _AnsiFlags)
								;
							}
						}
					}
				}
				break;
			case EEJSONType_Array:
				{
					CEJSON RawValue;
					if (_bStrict)
					{
						fg_CheckType(_Value, _Template.f_EType());
						RawValue = _Value.f_Array();
					}
					else if (_Value.f_IsArray())
						RawValue = _Value;
					else if (_Value.f_IsString())
					{
						CStr TrimmedValue = _Value.f_String().f_Trim();
						if (TrimmedValue.f_StartsWith("["))
						{
							RawValue = fp_ParseEJSON(_Value.f_String(), "Error parsing array parameter");
							if (!RawValue.f_IsArray())
								DMibError("Expected array");
						}
						else
						{
							if (_Value.f_String().f_IsEmpty())
								RawValue = EEJSONType_Array;
							else
								RawValue = _Value.f_String().f_SplitEscaped(',');
						}
					}
					else
						DMibError("Array can only be converted from string");

					auto &TemplateArray = _Template.f_Array();
					if (TemplateArray.f_IsEmpty())
					{
						Return = RawValue;
						break;
					}

					DMibCheck(TemplateArray.f_GetLen() == 1);

					auto &OutputArray = Return.f_Array();
					auto &InputArray = RawValue.f_Array();
					if (TemplateArray.f_GetLen() == 1)
					{
						auto &Template = TemplateArray.f_GetFirst();
						CStr Identifier = fg_Format("{}.[]", _Identifier);
						for (auto &InputElement : InputArray)
							OutputArray.f_Insert() = fp_ConvertValue(Template, InputElement, Identifier, true, _AnsiFlags);
					}

					Return = RawValue;
				}
				break;
			case EEJSONType_Null:
				if (_bStrict)
				{
					fg_CheckType(_Value, _Template.f_EType());
					Return = nullptr;
					break;
				}
				else if (_Value.f_IsNull())
				{
					Return = _Value;
					break;
				}
				else if (_Value.f_IsString())
				{
					if (_Value.f_String() == "null")
					{
						Return = nullptr;
						break;
					}
					else
						DMibError(fg_Format("Failed to parse {} as a null value", fs_Color("\"{}\""_f << _Value.f_String(), EColor_String, _AnsiFlags)));
				}
				else
					DMibError("Null can only be converted from string");
				break;
			default:
				DMibError("Invalid template type");
				break;
			}
		}
		catch (CCommandLineConvertException const &)
		{
			throw;
		}
		catch (NException::CException const &_Exception)
		{
			DMibCommandLineConvertException(fg_Format("{}: {}", _Identifier, _Exception.f_GetErrorStr()));
		}

		return Return;
	}

	template <typename t_CCustomization>
	NStr::CStr TCCommandLineSpecification<t_CCustomization>::CInternal::CValue::fp_FormatValue
		(
		 	NEncoding::CEJSON const &_Template
		 	, NEncoding::CEJSON const &_Value
		 	, NStr::CStr const &_Identifier
		 	, NCommandLine::EAnsiEncodingFlag _AnsiFlags
		) const
	{
		using namespace NEncoding;
		using namespace NStr;

		CStr Return;

		auto fToString = [&](CEJSON const &_Value)
			{
				return _Value.f_ToStringColored(_AnsiFlags, "    ");
			}
		;
		auto fColor = [&](CStr const &_Value, EColor _Color)
			{
				if (_AnsiFlags & NCommandLine::EAnsiEncodingFlag_Color)
					return fs_Color(_Value, _Color, _AnsiFlags);
				else
					return _Value;
			}
		;

		switch (_Template.f_EType())
		{
		case EEJSONType_String:
		case EEJSONType_Integer:
		case EEJSONType_Float:
		case EEJSONType_Boolean:
		case EEJSONType_Object:
		case EEJSONType_Array:
		case EEJSONType_Null:
			{
				Return = fToString(_Value);
			}
			break;
		case EEJSONType_Binary:
			{
				if (!_Value.f_IsBinary())
					Return = fToString(_Value);
				else
					Return = fColor(fg_Base64Encode(_Value.f_Binary()), EColor_Binary);
			}
			break;
		case EEJSONType_Date:
			{
				if (!_Value.f_IsDate())
				{
					Return = fToString(_Value);
					break;
				}

				Return = fColor("{}"_f << _Value.f_Date(), EColor_Date);
			}
			break;
		case EEJSONType_UserType:
			{
				if (_Value.f_IsBinary())
					Return = fColor(fg_Base64Encode(_Value.f_Binary()), EColor_Binary);
				else if (_Value.f_IsDate())
					Return = fColor("{}"_f << _Value.f_Date(), EColor_Date);
				else
					Return = fToString(_Value);
			}
			break;
		default:
			DMibError("Invalid template type");
			break;
		}

		return Return;
	}

	template <typename t_CCustomization>
	bool TCCommandLineSpecification<t_CCustomization>::CInternal::COption::f_IsEnabled(COptionSet const &_OptionSet, bool _bIsDirect) const
	{
		if (_OptionSet.m_Disallowed.f_FindEqual(this->m_Identifier))
			return false;
		if (m_bDefaultEnabled)
		{
			if (_OptionSet.m_bAllowedSpecified && !_OptionSet.m_Allowed.f_FindEqual(this->m_Identifier))
				return false;
		}
		else
		{
			if (!_OptionSet.m_Allowed.f_FindEqual(this->m_Identifier))
				return false;
		}

		if (!m_bValidForDirectCommand && _bIsDirect)
			return false;

		return true;
	}

	template <typename t_CCustomization>
	void TCCommandLineSpecification<t_CCustomization>::CInternal::COption::f_ParseOption(NEncoding::CEJSON const &_Option)
	{
		fs_CheckValidObject(_Option, {"Type", "Default", "Names", "Description", "DefaultEnabled", "Hidden", "CanNegate", "DisablesAllErrors", "ValidForDirectCommand"});

		CValue::f_Parse(_Option);

		if (auto *pValue = _Option.f_GetMember("DefaultEnabled"))
			m_bDefaultEnabled = pValue->f_Boolean();
		if (auto *pValue = _Option.f_GetMember("Hidden"))
			m_bHidden = pValue->f_Boolean();
		if (auto *pValue = _Option.f_GetMember("CanNegate"))
			m_bCanNegate = pValue->f_Boolean();
		if (auto *pValue = _Option.f_GetMember("DisablesAllErrors"))
			m_bDisablesAllErrors = pValue->f_Boolean();
		if (auto *pValue = _Option.f_GetMember("ValidForDirectCommand"))
			m_bValidForDirectCommand = pValue->f_Boolean();
	}

	template <typename t_CCustomization>
	void TCCommandLineSpecification<t_CCustomization>::CInternal::CParameter::f_ParseParameter(NEncoding::CEJSON const &_Parameter)
	{
		fs_CheckValidObject(_Parameter, {"Type", "Default", "Description"});
		CValue::f_Parse(_Parameter);

		if (m_bVector)
		{
			if (!this->m_TypeTemplate.f_IsArray())
				DMibError("A ... parameter needs to have an array type");

			if (this->m_TypeTemplate.f_Array().f_GetLen() != 1)
				DMibError("A ... parameter needs to have a specified type in the array");
		}
	}

	template <typename t_CCustomization>
	void TCCommandLineSpecification<t_CCustomization>::CInternal::fs_CheckValidObject(NEncoding::CEJSON const &_ToCheck, NContainer::TCSet<NStr::CStr> const &_AllowedKeys)
	{
		if (!_ToCheck.f_IsObject())
			DMibError("Command line description is not an object");

		for (auto &Member : _ToCheck.f_Object())
		{
			if (!_AllowedKeys.f_FindEqual(Member.f_Name()))
				DMibError(fg_Format("Unexpected member '{}'",Member.f_Name()));
		}
	}

	template <typename t_CCustomization>
	void TCCommandLineSpecification<t_CCustomization>::CInternal::f_CheckName(NStr::CStr const &_Name)
	{
		if (_Name.f_StartsWith("--no-"))
		{
			DMibError
				(
					"An option or command name cannot start with --no- as this is reserved for negating a boolean option."
					" If this is what you are trying to achieve, rather use then non-negative form and set the default to true."
				)
			;
		}
		if (_Name.f_FindChar('=') > 0)
			DMibError("An option or command name cannot contain '=' as this can be used to specify the value for an option.");
	}

	template <typename t_CCustomization>
	void TCCommandLineSpecification<t_CCustomization>::CInternal::CCommandCommon::f_RegisterOptions(CInternal &_Internal, NEncoding::CEJSON const &_Options)
	{
		auto &Internal = _Internal;

		for (auto &Option : _Options.f_Object())
		{
			bool bOptional = false;
			bool bVector = false;
			NStr::CStr Identifier = fg_ParseIdentifier(Option.f_Name(), bOptional, bVector);

			if (bVector)
				DMibError("... vector form is not supported for options");
			if (Identifier == "Command")
				DMibError("Option cannot by identified as 'Command'. This is reserved for name of command in params");
			if (Internal.m_GlobalOptionsByIdentifier.f_FindEqual(Identifier))
				DMibError(fg_Format("Option with same identifier '{}' already exists in global options", Identifier));
			if (m_pSection->m_SectionOptionsByIdentifier.f_FindEqual(Identifier))
				DMibError(fg_Format("Option with same identifier '{}' already exists in section options", Identifier));
			if (m_OptionsByIdentifier.f_FindEqual(Identifier))
				DMibError(fg_Format("Option with identifier '{}' already exists on command", Identifier));
			if (m_ParametersByIdentifier.f_FindEqual(Identifier))
				DMibError(fg_Format("Parameter with identifier '{}' already exists on command", Identifier));

			auto &NameArray = Option.f_Value()["Names"].f_Array();

			NContainer::TCVector<NStr::CStr> Names;
			for (auto &NameJSON : NameArray)
			{
				auto &Name = NameJSON.f_String();
				Internal.f_CheckName(Name);
				if (Internal.m_CommandByName.f_FindEqual(Name))
					DMibError(fg_Format("Name is already used as a command '{}'", Name));
				if (m_pSection->m_SectionOptionsByName.f_FindEqual(Name))
					DMibError(fg_Format("Option with same name '{}' already exists in section options", Name));
				if (Internal.m_GlobalOptionsByName.f_FindEqual(Name))
					DMibError(fg_Format("Option with same name '{}' already exists in global options", Name));
				if (m_OptionsByName.f_FindEqual(Name))
					DMibError(fg_Format("Option with name '{}' already exists on command", Name));
				Names.f_Insert(Name);
			}

			auto &NewOption = m_Options.f_Insert(fg_Construct(Identifier, Names, this, bOptional));
			m_OptionsByIdentifier[Identifier] = &NewOption;
			Internal.m_CommandOptionsIdentifiers[Identifier].f_Insert(m_Names.f_GetFirst());
			m_pSection->m_CommandOptionsIdentifiers[Identifier].f_Insert(m_Names.f_GetFirst());

			for (auto &NameJSON : NameArray)
			{
				auto &Name = NameJSON.f_String();
				Internal.m_CommandOptionsNames[Name].f_Insert(m_Names.f_GetFirst());
				m_OptionsByName[Name] = &NewOption;
			}

			NewOption.f_ParseOption(Option.f_Value());

			if (NameArray.f_IsEmpty() && !NewOption.m_bHidden)
				DMibError("You need to specify at least one name for option");

			if (!NameArray.f_IsEmpty() && NewOption.m_bHidden)
				DMibError("Hidden options cannot specify names");

			if (!NewOption.m_bDefaultEnabled)
				DMibError("Default enabled can only be disabled on section options");
		}
	}
}
