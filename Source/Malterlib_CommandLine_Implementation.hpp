// Copyright © 2019 Nonna Holding AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

#include <Mib/Encoding/JSONShortcuts>

namespace NMib::NCommandLine
{
	template <typename t_CCustomization>
	TCCommandLineSpecification<t_CCustomization>::CSectionCommon::CSectionCommon(CInternal *_pInternal, void *_pSection)
		: mp_pInternal(_pInternal)
		, mp_pSection(_pSection)
	{
	}

	template <typename t_CCustomization>
	TCCommandLineSpecification<t_CCustomization>::CCommand::CCommand(CInternal *_pInternal, void *_pCommand)
		: mp_pInternal(_pInternal)
		, mp_pCommand(_pCommand)
	{
	}

	template <typename t_CCustomization>
	void TCCommandLineSpecification<t_CCustomization>::CCommand::f_RegisterOptions(NEncoding::CEJSONOrdered &&_Options)
	{
		auto &Internal = *mp_pInternal;
		typename CInternal::CCommand *pCommand = fg_AutoStaticCast(mp_pCommand);
		pCommand->f_RegisterOptions(Internal, fg_Move(_Options));
	}

	template <typename tf_CIdentifiers>
	static NStr::CStr fg_GetFormattedIdentifiers(tf_CIdentifiers &_Identifiers)
	{
		NStr::CStr Commands;
		for (auto &CommandIdentifier : _Identifiers)
			fg_AddStrSep(Commands, CommandIdentifier, ",");
		return Commands;
	}

	template <typename t_CCustomization>
	void TCCommandLineSpecification<t_CCustomization>::CSectionCommon::f_RegisterSectionOptions(NEncoding::CEJSONOrdered &&_Options)
	{
		using namespace NStr;
		
		typename CInternal::CSection *pSection = fg_AutoStaticCast(mp_pSection);
		auto &Section = *pSection;
		auto &Internal = *mp_pInternal;

		for (auto &OptionMutable : _Options.f_Object())
		{
			auto &Option = fg_Const(OptionMutable);

			bool bOptional = false;
			bool bVector = false;
			CStr Identifier = fg_ParseIdentifier(Option.f_Name(), bOptional, bVector);

			if (bVector)
				DMibError("... vector form is not supported supported for options");
			if (Identifier == "Command")
				DMibError("Option cannot by identified as 'Command'. This is reserved for name of command in params");
			if (Internal.m_GlobalOptionsByIdentifier.f_FindEqual(Identifier))
				DMibError(fg_Format("A global option with identifier '{}' already exists", Identifier));
			if (Section.m_SectionOptionsByIdentifier.f_FindEqual(Identifier))
				DMibError(fg_Format("A section option with identifier '{}' already exists.", Identifier));
			if (auto pIdentifiers = Section.m_CommandOptionsIdentifiers.f_FindEqual(Identifier))
				DMibError(fg_Format("Option with same identifier '{}' already exists in command(s): {}", Identifier, fg_GetFormattedIdentifiers(*pIdentifiers)));
			if (auto pIdentifiers = Section.m_CommandParameterIdentifiers.f_FindEqual(Identifier))
				DMibError(fg_Format("Command parameter with same identifier '{}' already exists in command(s): {}", Identifier, fg_GetFormattedIdentifiers(*pIdentifiers)));

			auto &NameArray = Option.f_Value()["Names"].f_Array();
			if (NameArray.f_IsEmpty())
				DMibError("You need to specify at least one name for option");

			NContainer::TCVector<CStr> Names;
			for (auto &NameJSON : NameArray)
			{
				auto &Name = NameJSON.f_String();
				Internal.f_CheckName(Name);
				if (Internal.m_CommandByName.f_FindEqual(Name))
					DMibError(fg_Format("Name is already used as a command '{}'", Name));
				if (Internal.m_GlobalOptionsByName.f_FindEqual(Name))
					DMibError(fg_Format("A global option with same name '{}' already exists", Name));
				if (Section.m_SectionOptionsByName.f_FindEqual(Name))
					DMibError(fg_Format("A section option with name '{}' already exists", Name));
				if (auto pIdentifiers = Section.m_CommandOptionsNames.f_FindEqual(Name))
					DMibError(fg_Format("Option with name '{}' already exists in command(s): {}", Name, fg_GetFormattedIdentifiers(*pIdentifiers)));
				Names.f_Insert(Name);
			}

			auto &NewOption = Section.m_SectionOptions.f_Insert(fg_Construct(Identifier, Names, nullptr, bOptional));
			Section.m_SectionOptionsByIdentifier[Identifier] = &NewOption;
			Internal.m_SectionOptionsIdentifiers[Identifier].f_Insert(Section.m_Heading);

			for (auto &NameJSON : NameArray)
			{
				auto &Name = NameJSON.f_String();
				Section.m_SectionOptionsByName[Name] = &NewOption;
				Internal.m_SectionOptionsNames[Name].f_Insert(Section.m_Heading);
			}

			NewOption.f_ParseOption(fg_Move(OptionMutable.f_Value()));
		}
	}

	template <typename t_CCustomization>
	void TCCommandLineSpecification<t_CCustomization>::f_RegisterGlobalOptions(NEncoding::CEJSONOrdered &&_Options)
	{
		using namespace NStr;
		
		auto &Internal = *mp_pInternal;

		for (auto &OptionMutable : _Options.f_Object())
		{
			auto &Option = fg_Const(OptionMutable);
			bool bOptional = false;
			bool bVector = false;
			CStr Identifier = fg_ParseIdentifier(Option.f_Name(), bOptional, bVector);

			if (bVector)
				DMibError("... vector form is not supported supported for options");
			if (Identifier == "Command")
				DMibError("Option cannot by identified as 'Command'. This is reserved for name of command in params");
			if (Internal.m_GlobalOptionsByIdentifier.f_FindEqual(Identifier))
				DMibError(fg_Format("A global option with identifier '{}' already exists", Identifier));
			if (auto pIdentifiers = Internal.m_SectionOptionsIdentifiers.f_FindEqual(Identifier))
				DMibError(fg_Format("Section option with same identifier '{}' already exists in sections(s): {}", Identifier, fg_GetFormattedIdentifiers(*pIdentifiers)));
			if (auto pIdentifiers = Internal.m_CommandOptionsIdentifiers.f_FindEqual(Identifier))
				DMibError(fg_Format("Option with same identifier '{}' already exists in command(s): {}", Identifier, fg_GetFormattedIdentifiers(*pIdentifiers)));
			if (auto pIdentifiers = Internal.m_CommandParameterIdentifiers.f_FindEqual(Identifier))
				DMibError(fg_Format("Command parameter with same identifier '{}' already exists in command(s): {}", Identifier, fg_GetFormattedIdentifiers(*pIdentifiers)));

			auto &NameArray = Option.f_Value()["Names"].f_Array();
			if (NameArray.f_IsEmpty())
				DMibError("You need to specify at least one name for option");

			NContainer::TCVector<CStr> Names;
			for (auto &NameJSON : NameArray)
			{
				auto &Name = NameJSON.f_String();
				Internal.f_CheckName(Name);
				if (Internal.m_CommandByName.f_FindEqual(Name))
					DMibError(fg_Format("Name is already used as a command '{}'", Name));
				if (Internal.m_GlobalOptionsByName.f_FindEqual(Name))
					DMibError(fg_Format("A global option with same name '{}' already exists", Name));
				if (auto pIdentifiers = Internal.m_SectionOptionsNames.f_FindEqual(Name))
					DMibError(fg_Format("Section option with name '{}' already exists in sections(s): {}", Name, fg_GetFormattedIdentifiers(*pIdentifiers)));
				if (auto pIdentifiers = Internal.m_CommandOptionsNames.f_FindEqual(Name))
					DMibError(fg_Format("Option with name '{}' already exists in command(s): {}", Name, fg_GetFormattedIdentifiers(*pIdentifiers)));
				Names.f_Insert(Name);
			}

			auto &NewOption = Internal.m_GlobalOptions.f_Insert(fg_Construct(Identifier, Names, nullptr, bOptional));
			Internal.m_GlobalOptionsByIdentifier[Identifier] = &NewOption;

			for (auto &NameJSON : NameArray)
				Internal.m_GlobalOptionsByName[NameJSON.f_String()] = &NewOption;

			NewOption.f_ParseOption(fg_Move(OptionMutable.f_Value()));

			if (!NewOption.m_bDefaultEnabled)
				DMibError("Default enabled can only be disabled on section options");
		}
	}

	template <typename t_CCustomization>
	auto TCCommandLineSpecification<t_CCustomization>::CInternal::f_RegisterCommand(CSection &_Section, NEncoding::CEJSONOrdered &&_CommandDescription) -> CCommand *
	{
		using namespace NStr;

		auto CommandDescription = fg_Const(_CommandDescription);

		fs_CheckValidObject
			(
				CommandDescription
				,
				{
					"Names"
					, "Description"
					, "Status"
					, "Output"
					, "ErrorOnCommandAsParameter"
					, "GreedyDefaultCommand"
					, "GreedyDefaultCommandParameters"
					, "ErrorOnOptionAsParameter"
					, "ErrorOnOptionAsParameterWhenDefaultCommand"
					, "AlwaysVerbose"
					, "Parameters"
					, "Options"
					, "SectionOptions"
					, "DisableSectionOptions"
					, "Category"
					, "ShowOptionsInCommandEntry"
					, "ShowParametersStart"
					, "GlobalOptions"
					, "DisableGlobalOptions"
				}
			)
		;

		auto &NameArray = CommandDescription["Names"].f_Array();
		if (NameArray.f_IsEmpty())
			DMibError("You need to specify at least one name for command");

		NContainer::TCVector<CStr> Names;
		for (auto &NameJSON : NameArray)
		{
			auto &Name = NameJSON.f_String();
			f_CheckName(Name);
			if (m_GlobalOptionsByName.f_FindEqual(Name))
				DMibError(fg_Format("A global option with same name '{}' already exists", Name));
			if (auto pIdentifiers = m_SectionOptionsNames.f_FindEqual(Name))
				DMibError(fg_Format("Section option with name '{}' already exists in section(s): {}", Name, fg_GetFormattedIdentifiers(*pIdentifiers)));
			if (auto pIdentifiers = m_CommandOptionsNames.f_FindEqual(Name))
				DMibError(fg_Format("Option with name '{}' already exists in command(s): {}", Name, fg_GetFormattedIdentifiers(*pIdentifiers)));
			if (m_CommandByName.f_FindEqual(Name))
				DMibError(fg_Format("Name is already used for another command '{}'", Name));
			Names.f_Insert(Name);
		}

		auto &NewCommand = _Section.m_Commands.f_Insert(fg_Construct(&_Section, Names));

		for (auto &NameJSON : NameArray)
		{
			auto &Name = NameJSON.f_String();
			m_CommandByName[Name] = &NewCommand;
		}

		fg_ParseDescription(CommandDescription, NewCommand.m_ShortDescription, NewCommand.m_LongDescription);

		if (auto *pStatus = CommandDescription.f_GetMember("Status"))
		{
			for (auto &Status : pStatus->f_Object())
				NewCommand.m_StatusDescription[Status.f_Name()] = Status.f_Value().f_String();
		}
		if (auto *pOutput = CommandDescription.f_GetMember("Output"))
			NewCommand.m_OutputDescription = pOutput->f_String();
		if (auto *pCategory = CommandDescription.f_GetMember("Category"))
			NewCommand.m_Category = pCategory->f_String();
		if (auto *pErrorOnCommandAsParameter = CommandDescription.f_GetMember("ErrorOnCommandAsParameter"))
			NewCommand.m_bErrorOnCommandAsParameter = pErrorOnCommandAsParameter->f_Boolean();
		if (auto *pShowOptionsInCommandEntry = CommandDescription.f_GetMember("ShowOptionsInCommandEntry"))
			NewCommand.m_bShowOptionsInCommandEntry = pShowOptionsInCommandEntry->f_Boolean();
		if (auto *pShowParametersStart = CommandDescription.f_GetMember("ShowParametersStart"))
			NewCommand.m_bShowParametersStart = pShowParametersStart->f_Boolean();
		if (auto *pGreedyDefaultCommand = CommandDescription.f_GetMember("GreedyDefaultCommand"))
			NewCommand.m_bGreedyDefaultCommand = pGreedyDefaultCommand->f_Boolean();
		if (auto *pGreedyDefaultCommandParameters = CommandDescription.f_GetMember("GreedyDefaultCommandParameters"))
			NewCommand.m_bGreedyDefaultCommandParameters = pGreedyDefaultCommandParameters->f_Boolean();
		if (auto *pErrorOnOptionAsParameter = CommandDescription.f_GetMember("ErrorOnOptionAsParameter"))
			NewCommand.m_bErrorOnOptionAsParameter = pErrorOnOptionAsParameter->f_Boolean();
		if (auto *pErrorOnOptionAsParameterWhenDefaultCommand = CommandDescription.f_GetMember("ErrorOnOptionAsParameterWhenDefaultCommand"))
			NewCommand.m_bErrorOnOptionAsParameterWhenDefaultCommand = pErrorOnOptionAsParameterWhenDefaultCommand->f_Boolean();
		if (auto *pAlwaysVerbose = CommandDescription.f_GetMember("AlwaysVerbose"))
			NewCommand.m_bAlwaysVerbose = pAlwaysVerbose->f_Boolean();
		if (auto *pValue = CommandDescription.f_GetMember("SectionOptions"))
		{
			NewCommand.m_SectionOptionSet.m_bAllowedSpecified = true;
			for (auto &Option : pValue->f_Array())
				NewCommand.m_SectionOptionSet.m_Allowed[Option.f_String()];
		}
		if (auto *pValue = CommandDescription.f_GetMember("DisableSectionOptions"))
		{
			for (auto &Option : pValue->f_Array())
				NewCommand.m_SectionOptionSet.m_Disallowed[Option.f_String()];
		}
		if (auto *pValue = CommandDescription.f_GetMember("GlobalOptions"))
		{
			NewCommand.m_GlobalOptionSet.m_bAllowedSpecified = true;
			for (auto &Option : pValue->f_Array())
				NewCommand.m_GlobalOptionSet.m_Allowed[Option.f_String()];
		}
		if (auto *pValue = CommandDescription.f_GetMember("DisableGlobalOptions"))
		{
			for (auto &Option : pValue->f_Array())
				NewCommand.m_GlobalOptionSet.m_Disallowed[Option.f_String()];
		}

		if (auto *pParameters = _CommandDescription.f_GetMember("Parameters"))
		{
			bool bWasOptional = false;
			bool bWasVector = false;
			for (auto &ParameterMutable : pParameters->f_Object())
			{
				auto &Parameter = fg_Const(ParameterMutable);
				bool bOptional = false;
				bool bVector = false;
				CStr Identifier = fg_ParseIdentifier(Parameter.f_Name(), bOptional, bVector);

				if (bWasVector)
					DMibError("Previous parameter was a vector so no further parameters can be specified.");
				if (bWasOptional && !bOptional)
					DMibError("Previous parameter was optional, but this one is not. This does not make sense.");
				if (Identifier == "Command")
					DMibError("Parameter cannot by identified as 'Command'. This is reserved for name of command in params");

				bWasVector = bVector;
				bWasOptional = bOptional;
				if (NewCommand.m_ParametersByIdentifier.f_FindEqual(Identifier))
					DMibError(fg_Format("Duplicate parameter identifier '{}'", Identifier));
				if (m_GlobalOptionsByIdentifier.f_FindEqual(Identifier))
					DMibError(fg_Format("Option with same identifier '{}' already exists in global options", Identifier));
				if (_Section.m_SectionOptionsByIdentifier.f_FindEqual(Identifier))
					DMibError(fg_Format("Option with same identifier '{}' already exists in section options", Identifier));

				auto &NewParameter = NewCommand.m_Parameters.f_Insert(fg_Construct(Identifier, bOptional, bVector));
				NewCommand.m_ParametersByIdentifier[Identifier] = &NewParameter;
				m_CommandParameterIdentifiers[Identifier].f_Insert(NewCommand.m_Names.f_GetFirst());
				_Section.m_CommandParameterIdentifiers[Identifier].f_Insert(NewCommand.m_Names.f_GetFirst());

				NewParameter.f_ParseParameter(fg_Move(ParameterMutable.f_Value()));
			}
		}

		if (auto *pOptions = _CommandDescription.f_GetMember("Options"))
			NewCommand.f_RegisterOptions(*this, fg_Move(*pOptions));

		return &NewCommand;
	}

	template <typename t_CCustomization>
	auto TCCommandLineSpecification<t_CCustomization>::CSectionCommon::f_RegisterDirectCommand
		(
			NEncoding::CEJSONOrdered &&_CommandDescription
			, NFunction::TCFunctionMovable<uint32 (NEncoding::CEJSONSorted const &_Parameters, CCommandLineClient &_CommandLineClient)> &&_fRunCommand
		)
		-> CCommand
	{
		typename CInternal::CSection *pSection = fg_AutoStaticCast(mp_pSection);
		auto &Section = *pSection;
		auto &Internal = *mp_pInternal;
		auto *pCommand = Internal.f_RegisterCommand(Section, fg_Move(_CommandDescription));
		pCommand->m_pDirectRunCommand = fg_Construct(fg_Move(_fRunCommand));
		return CCommand(mp_pInternal, pCommand);
	}

	template <typename t_CCustomization>
	TCCommandLineSpecification<t_CCustomization>::TCCommandLineSpecification()
		: mp_pInternal(fg_Construct())
	{
		using namespace NStr;
		f_AddSection("", "");

		f_RegisterGlobalOptions
			(
				{
					"MalterlibCommand?"_o=
					{
						"Names"_o= {"--malterlib-command-BA49ADC8-6CAA-4E8C-BA13-3A9273859D89"}
						, "Type"_o= ""
						, "Description"_o= "Override the command to run anywhere on the command line."
						, "Hidden"_o= true
					}
				}
			)
		;
	}

	template <typename t_CCustomization>
	TCCommandLineSpecification<t_CCustomization>::~TCCommandLineSpecification() noexcept
	{
	}

	template <typename t_CCustomization>
	TCCommandLineSpecification<t_CCustomization>::TCCommandLineSpecification(TCCommandLineSpecification const &_Other)
		: mp_pInternal(fg_Construct(*_Other.mp_pInternal))
	{
	}

	template <typename t_CCustomization>
	TCCommandLineSpecification<t_CCustomization> &TCCommandLineSpecification<t_CCustomization>::operator =(TCCommandLineSpecification const &_Other)
	{
		mp_pInternal = fg_Construct(*_Other.mp_pInternal);
		return *this;
	}

	template <typename t_CCustomization>
	TCCommandLineSpecification<t_CCustomization>::TCCommandLineSpecification(TCCommandLineSpecification &&_Other) = default;

	template <typename t_CCustomization>
	TCCommandLineSpecification<t_CCustomization> &TCCommandLineSpecification<t_CCustomization>::operator =(TCCommandLineSpecification &&_Other) = default;

	template <typename t_CCustomization>
	auto TCCommandLineSpecification<t_CCustomization>::f_GetDefaultSection() -> CSection
	{
		auto &Internal = *mp_pInternal;
		return CSection(&Internal, &Internal.m_Sections.f_GetFirst());
	}

	template <typename t_CCustomization>
	void TCCommandLineSpecification<t_CCustomization>::f_SetProgramDescription(NStr::CStr const &_Heading, NStr::CStr const &_Description)
	{
		auto &Internal = *mp_pInternal;
		auto &DefaultSection = Internal.m_Sections.f_GetFirst();
		DefaultSection.m_Heading = _Heading;
		DefaultSection.m_Description = _Description;
	}

	template <typename t_CCustomization>
	auto TCCommandLineSpecification<t_CCustomization>::f_AddSection(NStr::CStr const &_Heading, NStr::CStr const &_Description, NStr::CStr const &_AfterSection) -> CSection
	{
		auto &Internal = *mp_pInternal;
		typename CInternal::CSection *pNewSection;
		if (_AfterSection.f_IsEmpty())
			pNewSection = &Internal.m_Sections.f_Insert();
		else
		{
			NStr::CStr SectionToFind = _AfterSection == "Default" ? NStr::CStr() : _AfterSection;
			typename CInternal::CSection *pPreviousSection = nullptr;
			for (auto &Section : Internal.m_Sections)
			{
				if (Section.m_Heading == SectionToFind)
				{
					pPreviousSection = &Section;
					break;
				}
			}
			if (!pPreviousSection)
				pNewSection = &Internal.m_Sections.f_Insert();
			else
				pNewSection = &Internal.m_Sections.f_InsertAfter(*pPreviousSection);
		}

		auto &NewSection = *pNewSection;
		NewSection.m_Heading = _Heading;
		NewSection.m_Description = _Description;
		return CSection(&Internal, &NewSection);
	}

	template <typename t_CCustomization>
	void TCCommandLineSpecification<t_CCustomization>::f_SetDefaultCommand(CCommand const &_Command)
	{
		auto &Internal = *mp_pInternal;
		Internal.m_pDefaultCommand = fg_AutoStaticCast(_Command.mp_pCommand);
	}

	template <typename t_CCustomization>
	auto TCCommandLineSpecification<t_CCustomization>::f_AccessInternal() -> CInternal &
	{
		return *mp_pInternal;
	}
}
