// Copyright © 2015 Hansoft AB 
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#include "Malterlib_CommandLine.h"
#include "Malterlib_CommandLine_Implementation.h"
#include <Mib/String/FuzzyMatch>

namespace NMib::NCommandLine
{
	template <typename t_CCustomization>
	auto TCCommandLineSpecification<t_CCustomization>::f_ParseCommandLine(NContainer::TCVector<NStr::CStr> &&_Params, NCommandLine::EAnsiEncodingFlag _AnsiFlags) -> CParsedCommandLine
	{
		using namespace NEncoding;
		using namespace NStr;
		using namespace NContainer;

		auto &CommandLineSpec = *mp_pInternal;

		CEJsonSorted CommandParams = EJsonType_Object;

		for (auto &Option : CommandLineSpec.m_GlobalOptions)
		{
			if (Option.m_Default.f_IsValid())
				CommandParams[Option.m_Identifier] = Option.m_Default;
		}

		typename CInternal::CCommand const *pFoundCommand = nullptr;
		typename CInternal::CCommand const *pCurrentCommand = CommandLineSpec.m_pDefaultCommand;

		bool bDisableAllErrors = false;
		NStr::CStr FoundCommandName;

		decltype(pFoundCommand->m_Parameters.f_GetIterator()) iCommandParameter;

		bool bDefaultCommandUsed = false;

		NStr::CStr ProgramName;
		bool bParametersOnly = false;

		auto fColor = [&](CStr const &_String, typename CInternal::EColor _Color) -> CStr
			{
				return CInternal::fs_Color(_String, _Color, _AnsiFlags);
			}
		;
		auto fColorValue = [&](typename CInternal::EColor _Color) -> typename CInternal::EColor
			{
				return _Color;
			}
		;

		auto fApplyOptionDefaults = [&](typename CInternal::CCommand const &_Command)
			{
				for (auto &Option : _Command.m_pSection->m_SectionOptions)
				{
					if (Option.m_Default.f_IsValid())
						CommandParams[Option.m_Identifier] = Option.m_Default;
				}

				for (auto &Option : _Command.m_Options)
				{
					if (Option.m_Default.f_IsValid())
						CommandParams[Option.m_Identifier] = Option.m_Default;
				}
			}
		;

		auto fUseDefaultCommand = [&]()
			{
				if (pFoundCommand)
					return;
				if (!bDefaultCommandUsed && pCurrentCommand)
				{
					fApplyOptionDefaults(*pCurrentCommand);
					bDefaultCommandUsed = true;
				}
			}
		;

		auto fFoundCommand = [&](typename CInternal::CCommand const *_pCommand, CStr const &_CommandName)
			{
				pCurrentCommand = pFoundCommand = _pCommand;
				FoundCommandName = _CommandName;
				iCommandParameter = pFoundCommand->m_Parameters.f_GetIterator();

				fApplyOptionDefaults(*pFoundCommand);
			}
		;

		TCVector<CStr> Params;

		for (auto iParameter = _Params.f_GetIterator(); iParameter; ++iParameter)
		{
			if (*iParameter == "--malterlib-command-BA49ADC8-6CAA-4E8C-BA13-3A9273859D89")
			{
				++iParameter;
				if (!iParameter)
					DMibError("--malterlib-command-BA49ADC8-6CAA-4E8C-BA13-3A9273859D89 specified without specifying command");

				auto *pCommand = CommandLineSpec.m_CommandByName.f_FindEqual(*iParameter);
				if (!pCommand)
					DMibError("Command '{}' specified for --malterlib-command-BA49ADC8-6CAA-4E8C-BA13-3A9273859D89 could not be found"_f << *iParameter);

				fFoundCommand(*pCommand, *iParameter);
			}
			else
				Params.f_Insert(fg_Move(*iParameter));
		}

		if (pCurrentCommand && pCurrentCommand->m_bGreedyDefaultCommand)
			fFoundCommand(pCurrentCommand, pCurrentCommand->m_Names.f_GetFirst());
		else if (pCurrentCommand)
			iCommandParameter = pCurrentCommand->m_Parameters.f_GetIterator();

		typename CInternal::COption *pCurrentOption = nullptr;
		CStr CurrentOptionName;
		typename CInternal::EColor CurrentOptionColor = CInternal::EColor_Option;

		auto fParseValue = [&](typename CInternal::CValue const &_Value, CStr const &_StringData, typename CInternal::EColor _Color, CStr const &_OptionName = {})
			{
				if (_OptionName.f_IsEmpty())
					CommandParams[_Value.m_Identifier] = _Value.f_ConvertValue(_StringData, fColorValue(_Color), _AnsiFlags);
				else
					CommandParams[_Value.m_Identifier] = _Value.f_ConvertValue(_StringData, _OptionName, fColorValue(_Color), _AnsiFlags);
			}
		;

		bool bUsedVectorParam = false;
		bool bFatalError = false;

		TCVector<NException::CExceptionPointer> Exceptions;
		TCVector<NFunction::TCFunctionMovable<void ()>> ChecksToPerform;

		for (auto &Parameter : Params)
		{
			if (ProgramName.f_IsEmpty())
			{
				ProgramName = Parameter;
				continue;
			}

			if (Parameter == "--")
			{
				bParametersOnly = true;
				continue;
			}

			try
			{
				if (!bParametersOnly)
				{
					if (pCurrentOption)
					{
						fParseValue(*pCurrentOption, Parameter, CurrentOptionColor, CurrentOptionName);
						pCurrentOption = nullptr;
						CurrentOptionName.f_Clear();
						continue;
					}

					CStr ParsedParameter = Parameter;
					CStr OptionValue;
					bool bOptionValueSet = false;
					bool bNegatedOption = false;

					if (Parameter.f_StartsWith("--no-"))
					{
						ParsedParameter = "--" + Parameter.f_Extract(5);
						bOptionValueSet = true;
						OptionValue = "false";
						bNegatedOption = true;
					}
					else
					{
						aint iParameterAssign = Parameter.f_FindChar('=');
						if (iParameterAssign >= 0)
						{
							OptionValue = Parameter.f_Extract(iParameterAssign + 1);
							bOptionValueSet = true;
							ParsedParameter = Parameter.f_Left(iParameterAssign);
						}
					}

					auto fCheckCommand = [&](CStr const &_ParsedParameter)
						{
							auto *pCommand = CommandLineSpec.m_CommandByName.f_FindEqual(_ParsedParameter);
							if (pCommand)
							{
								if (bOptionValueSet)
									DMibError(fg_Format("You cannot specify a command parameter with '=': {}", OptionValue));
								if (pFoundCommand)
								{
									if (pFoundCommand->m_bErrorOnCommandAsParameter)
									{
										DMibError
											(
												"Command {} already specified. You cannot specify additional command {}"_f
												<< fColor(FoundCommandName, CInternal::EColor_Command)
												<< fColor(_ParsedParameter, CInternal::EColor_Command)
											)
										;
									}
								}
								else
								{
									if (bDefaultCommandUsed)
									{
										DMibError
											(
												fg_Format
												(
													"Options or parameters have already been parsed for the default command. You should specify {} first on the command line"
													, fColor(_ParsedParameter, CInternal::EColor_Command)
												)
											)
										;
									}

									fFoundCommand(*pCommand, _ParsedParameter);
									return true;
								}
							}
							return false;
						}
					;

					if (fCheckCommand(ParsedParameter))
						continue;

					auto fParseOption = [&](typename CInternal::COption const &_Value, CStr const &_OptionName, typename CInternal::EColor _Color)
						{
							if (_Value.m_TypeTemplate.f_IsBoolean())
							{
								if (bNegatedOption && !_Value.m_bCanNegate)
									DMibError(fg_Format("Option {} cannot be negated with --no-", fColor(_Value.m_Names.f_GetFirst(), _Color)));

								if (!bOptionValueSet)
								{
									OptionValue = "true";
									bOptionValueSet = true;
								}
							}
							else if (bNegatedOption)
								DMibError(fg_Format("You cannot negate a non-boolean option {} with --no-", fColor(_Value.m_Names.f_GetFirst(), _Color)));

							if (_Value.m_bDisablesAllErrors)
								bDisableAllErrors = true;

							if (bOptionValueSet)
							{
								fParseValue(_Value, OptionValue, CurrentOptionColor, _OptionName);
								return true;
							}
							return false;
						}
					;

					auto fCheckOptions = [&](CStr const &_ParsedParameter, bool _bOnlyShortBooleans)
						{
							auto fCheckShortBooleans = [&](typename CInternal::COption const &_Option)
								{
									if (!_bOnlyShortBooleans)
										return false;

									if (!_Option.m_TypeTemplate.f_IsBoolean())
										return true;

									if (_Option.m_Default.f_IsValid() && _Option.m_Default.f_Boolean())
										return true; // Only options with defaults not set or false can be turned on

									return false;
								}
							;
							if (pCurrentCommand)
							{
								if (auto *pOption = pCurrentCommand->m_OptionsByName.f_FindEqual(_ParsedParameter))
								{
									if (fCheckShortBooleans(**pOption))
										return false;

									fUseDefaultCommand();
									if (fParseOption(**pOption, _ParsedParameter, CInternal::EColor_Option))
										return true;

									DMibCheck(!_bOnlyShortBooleans);

									pCurrentOption = *pOption;
									CurrentOptionName = _ParsedParameter;
									CurrentOptionColor = CInternal::EColor_Option;
									return true;
								}
								if (auto *pOption = pCurrentCommand->m_pSection->m_SectionOptionsByName.f_FindEqual(_ParsedParameter))
								{
									if (fCheckShortBooleans(**pOption))
										return false;

									if (!(*pOption)->f_IsEnabled(pCurrentCommand->m_SectionOptionSet, !!pCurrentCommand->m_pDirectRunCommand))
									{
										DMibError
											(
												"Option {} is not allowed for command {}"_f
												<< fColor((*pOption)->m_Names.f_GetFirst(), CInternal::EColor_SectionOption)
												<< fColor(pCurrentCommand->m_Names.f_GetFirst(), CInternal::EColor_Command)
											)
										;
									}

									fUseDefaultCommand();
									if (fParseOption(**pOption, _ParsedParameter, CInternal::EColor_SectionOption))
										return true;

									DMibCheck(!_bOnlyShortBooleans);

									pCurrentOption = *pOption;
									CurrentOptionName = _ParsedParameter;
									CurrentOptionColor = CInternal::EColor_SectionOption;
									return true;
								}
							}

							if (auto *pOption = CommandLineSpec.m_GlobalOptionsByName.f_FindEqual(_ParsedParameter))
							{
								if (fCheckShortBooleans(**pOption))
									return false;

								ChecksToPerform.f_Insert
									(
										[pOption, &pCurrentCommand, &fColor]
										{
											if (pCurrentCommand)
											{
												if (!(*pOption)->f_IsEnabled(pCurrentCommand->m_GlobalOptionSet, !!pCurrentCommand->m_pDirectRunCommand))
												{
													DMibError
														(
															"Option {} is not allowed for command {}"_f
															<< fColor((*pOption)->m_Names.f_GetFirst(), CInternal::EColor_GlobalOption)
															<< fColor(pCurrentCommand->m_Names.f_GetFirst(), CInternal::EColor_Command)
														)
													;
												}
											}
										}
									)
								;

								if (fParseOption(**pOption, _ParsedParameter, CInternal::EColor_GlobalOption))
									return true;

								DMibCheck(!_bOnlyShortBooleans);

								pCurrentOption = *pOption;
								CurrentOptionName = _ParsedParameter;
								CurrentOptionColor = CInternal::EColor_GlobalOption;
								return true;
							}

							return false;
						}
					;

					if (fCheckOptions(ParsedParameter, false))
						continue;

					bool bForceFail = false;
					if (ParsedParameter.f_StartsWith("-") && !ParsedParameter.f_StartsWith("--"))
					{
						CUStr ToParse = ParsedParameter;
						auto *pParse = ToParse.f_GetStr() + 1;
						bool bFoundParam = false;

						while (*pParse)
						{
							CUStr ToTest = "-";
							ToTest.f_AddChar(*pParse);

							if (!fCheckCommand(ToTest))
							{
								if (!fCheckOptions(ToTest, true))
								{
									if (fCheckOptions(ToTest, false))
									{
										// Last param allowed to continue
										if (pParse[1] == 0)
											break;

										DMibError
											(
												"Option {} needs a value so needs to be last in list of short options"_f
												<< fColor(ToTest, CurrentOptionColor)
											)
										;
									}

									if (bFoundParam)
									{
										bFoundParam = false;
										bForceFail = true;
										ParsedParameter = ToTest;
									}
									break;
								}

								OptionValue.f_Clear();
								bOptionValueSet = false;
							}

							bFoundParam = true;
							++pParse;
						}

						if (bFoundParam)
							continue;
					}

					if
						(
							bForceFail
							||
							(
								Parameter.f_StartsWith("-")
								&& (!pFoundCommand || pFoundCommand->m_bErrorOnOptionAsParameter)
								&& (!pCurrentCommand || pCurrentCommand->m_bErrorOnOptionAsParameterWhenDefaultCommand)
							)
							|| (!Parameter.f_StartsWith("-") && !pFoundCommand && (!pCurrentCommand || !pCurrentCommand->m_bGreedyDefaultCommandParameters || !iCommandParameter))
						)
					{
						struct CFuzzyEntry
						{
							CStr m_Name;
							CStr m_ColoredName;
							fp64 m_Score;

							auto operator <=> (CFuzzyEntry const &_Right) const
							{
								return NStorage::fg_TupleReferences(m_Score, m_Name) <=> NStorage::fg_TupleReferences(_Right.m_Score, _Right.m_Name);
							}
						};

						TCVector<CFuzzyEntry> FuzzyEntries;

						auto fCheckName = [&](CStr const &_Name, typename CInternal::EColor _Color)
							{
								fp64 Score = NStr::fg_FuzzyMatchString(_Name, Parameter);
								auto &Entry = FuzzyEntries.f_Insert();
								Entry.m_Name = _Name;
								Entry.m_Score = Score;
								Entry.m_ColoredName = fColor(_Name, _Color);
							}
						;

						if (!pFoundCommand)
						{
							bFatalError = true;
							for (auto iName = CommandLineSpec.m_CommandByName.f_GetIterator(); iName; ++iName)
								fCheckName(iName.f_GetKey(), CInternal::EColor_Command);
						}

						if (pCurrentCommand)
						{
							for (auto iName = pCurrentCommand->m_OptionsByName.f_GetIterator(); iName; ++iName)
								fCheckName(iName.f_GetKey(), CInternal::EColor_Option);

							for (auto iName = pCurrentCommand->m_pSection->m_SectionOptionsByName.f_GetIterator(); iName; ++iName)
								fCheckName(iName.f_GetKey(), CInternal::EColor_SectionOption);
						}

						for (auto &Option : CommandLineSpec.m_GlobalOptions)
						{
							for (auto &Name : Option.m_Names)
								fCheckName(Name, CInternal::EColor_GlobalOption);
						}

						FuzzyEntries.f_Sort();

						CStr Error = fg_Format("No such option or command: {}", fColor(ParsedParameter, CInternal::EColor_Error));
						if (!FuzzyEntries.f_IsEmpty())
						{
							CStr EntryNames;
							fp64 BestScore = FuzzyEntries.f_GetFirst().m_Score;
							for (auto &Entry : FuzzyEntries)
							{
	#if 1
								if ((Entry.m_Score - BestScore) > 0.2)
									break;
								fg_AddStrSep(EntryNames, fg_Format("   {}", Entry.m_ColoredName) ,"\n");
	#else
								if ((Entry.m_Score - BestScore) > 0.2)
									fg_AddStrSep(EntryNames, fg_Format("-- {fe2} {}", Entry.m_Score, Entry.m_ColoredName) ,"\n");
								else
									fg_AddStrSep(EntryNames, fg_Format("   {fe2} {}", Entry.m_Score, Entry.m_ColoredName) ,"\n");
	#endif
							}
							if (!EntryNames.f_IsEmpty())
								Error += ". Did you mean any of the following?\n{}"_f << EntryNames;
						}

						DMibError(Error);
					}
				}

				if (!iCommandParameter)
					DMibError(fg_Format("Unexpected parameter value: {}", fColor(Parameter, CInternal::EColor_Error)));

				fUseDefaultCommand();
				if (iCommandParameter->m_bVector)
				{
					bUsedVectorParam = true;
					iCommandParameter->f_AppendConvertValue(CommandParams[iCommandParameter->m_Identifier], Parameter, fColorValue(CInternal::EColor_Parameter), _AnsiFlags);
				}
				else
				{
					fParseValue(*iCommandParameter, Parameter, CInternal::EColor_Parameter);
					++iCommandParameter;
				}
			}
			catch (NException::CException const &_Exception)
			{
				Exceptions.f_Insert(_Exception.f_ExceptionPointer());
				if (!bParametersOnly)
				{
					pCurrentOption = nullptr;
					CurrentOptionName.f_Clear();
				}
				if (bFatalError)
					break;
			}
		}

		fUseDefaultCommand();

		for (auto &fCheck : ChecksToPerform)
			fCheck();
		ChecksToPerform.f_Clear();

		bool bFoundCommand = !!pFoundCommand;

		if (!pFoundCommand)
			pFoundCommand = CommandLineSpec.m_pDefaultCommand;

		if (!pFoundCommand)
			Exceptions.f_Insert(fg_MakeException(DMibErrorInstance("No command specified")));

		if (pCurrentOption)
			Exceptions.f_Insert(fg_MakeException(DMibErrorInstance("Missing parameter for option: {}"_f << fColor(CurrentOptionName, CurrentOptionColor))));

		auto fCheckOption = [&](typename CInternal::COption const &_Option, typename CInternal::EColor _Color)
			{
				if (!_Option.m_bOptional && !CommandParams.f_GetMember(_Option.m_Identifier))
					Exceptions.f_Insert(fg_MakeException(DMibErrorInstance("Missing required option: {}"_f << fColor(_Option.m_Names.f_GetFirst(), _Color))));
			}
		;
		for (auto &Option : CommandLineSpec.m_GlobalOptions)
			fCheckOption(Option, CInternal::EColor_GlobalOption);
		if (pFoundCommand)
		{
			for (auto &Option : pFoundCommand->m_pSection->m_SectionOptions)
				fCheckOption(Option, CInternal::EColor_SectionOption);
			for (auto &Option : pFoundCommand->m_Options)
				fCheckOption(Option, CInternal::EColor_Option);
		}

		CStr MissingParameters;
		while (iCommandParameter && !iCommandParameter->m_bOptional && !bUsedVectorParam)
		{
			fg_AddStrSep(MissingParameters, fColor(iCommandParameter->m_Identifier, CInternal::EColor_Parameter), " ");
			++iCommandParameter;
		}

		if (!MissingParameters.f_IsEmpty())
			Exceptions.f_Insert(fg_MakeException(DMibErrorInstance("Missing required command parameters: {}"_f << MissingParameters)));

		for (; iCommandParameter; ++iCommandParameter)
		{
			if (iCommandParameter->m_bVector && bUsedVectorParam)
				break;
			if (iCommandParameter->m_Default.f_IsValid())
				CommandParams[iCommandParameter->m_Identifier] = iCommandParameter->m_Default;
		}

		if (!Exceptions.f_IsEmpty() && (!pFoundCommand || !bDisableAllErrors))
		{
			NException::CExceptionExceptionVectorData::CErrorCollector ErrorCollector;

			for (auto &Exception : Exceptions)
				ErrorCollector.f_AddError(fg_Move(Exception));

			if (bFoundCommand)
				ErrorCollector.f_AddError(fg_MakeException(DMibErrorInstance("\n\nTo see command syntax, run command with {}\n"_f << fColor("-?", CInternal::EColor_GlobalOption))));

			std::rethrow_exception(fg_Move(ErrorCollector).f_GetException());
		}

		if (pFoundCommand)
		{
			CommandParams["Command"] = pFoundCommand->m_Names.f_GetFirst();
			return {pFoundCommand->m_Names.f_GetFirst(), fg_Move(CommandParams)};
		}
		else
			return {"", fg_Move(CommandParams)};
	}
}
