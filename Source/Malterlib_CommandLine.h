// Copyright © 2019 Nonna Holding AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

#include <Mib/Encoding/EJSON>

namespace NMib::NCommandLine
{
	struct COneOf
	{
		inline_always COneOf(NEncoding::CEJSON const &_Config);
		template <typename ...tfp_CParams>
		inline_always COneOf(tfp_CParams const &...p_Config);

		inline_always operator NEncoding::CEJSON () &&;
		inline_always operator NEncoding::CEJSON () const &;

		NEncoding::CEJSON m_Config;
	};

	struct COneOfType
	{
		inline_always COneOfType(NEncoding::CEJSON const &_Config);
		template <typename ...tfp_CParams>
		inline_always COneOfType(tfp_CParams const &...p_Config);

		inline_always operator NEncoding::CEJSON () &&;
		inline_always operator NEncoding::CEJSON () const &;

		NEncoding::CEJSON m_Config;
	};

	template <typename t_CCustomization, typename t_CThis = void>
	struct TCCommandLineClient;

	struct CCommandLineSpecificationNoCustomization
	{
		template <typename t_CCommandLineSpecification>
		struct TCSection
		{
			using CSection = typename t_CCommandLineSpecification::CSectionCommon;
		};

		template <typename t_CCommand>
		struct TCInternalCommand
		{
			using CCommand = t_CCommand;
		};

		using CCommandLineClient = TCCommandLineClient<CCommandLineSpecificationNoCustomization>;
	};

	template <typename t_CCustomization>
	struct TCCommandLineSpecification
	{
		struct CInternal;
		struct CSectionCommon;

		using CCommandLineClient = typename t_CCustomization::CCommandLineClient;
		using CSection = typename t_CCustomization::template TCSection<TCCommandLineSpecification>::CSection;

		struct CCommand
		{
			friend struct TCCommandLineSpecification;
			friend struct CSectionCommon;
			friend CSection;

			void f_RegisterOptions(NEncoding::CEJSON const &_Options);
		private:
			CCommand(CInternal *_pInternal, void *_pCommand);
			void *mp_pCommand;
			CInternal *mp_pInternal;
		};

		struct CSectionCommon
		{
			friend struct TCCommandLineSpecification;
			friend struct TCCommandLineSpecification;
			void f_RegisterSectionOptions(NEncoding::CEJSON const &_Options);

			CCommand f_RegisterDirectCommand
				(
					NEncoding::CEJSON const &_CommandDescription
					, NFunction::TCFunctionMovable<uint32 (NEncoding::CEJSON const &_Parameters, CCommandLineClient &_CommandLineClient)> &&_fRunCommand
				)
			;

		protected:
			CSectionCommon(CInternal *_pInternal, void *_pSection);
			void *mp_pSection;
			CInternal *mp_pInternal;
		};

		struct CParsedCommandLine
		{
			NStr::CStr m_Command;
			NEncoding::CEJSON m_Params;
		};

		TCCommandLineSpecification();
		~TCCommandLineSpecification() noexcept;

		TCCommandLineSpecification(TCCommandLineSpecification const &_Other);
		TCCommandLineSpecification(TCCommandLineSpecification &&_Other);
		TCCommandLineSpecification &operator =(TCCommandLineSpecification const &_Other);
		TCCommandLineSpecification &operator =(TCCommandLineSpecification &&_Other);

		CSection f_GetDefaultSection();
		CSection f_AddSection(NStr::CStr const &_Heading, NStr::CStr const &_Description, NStr::CStr const &_AfterSection = {});
		static NContainer::TCVector<NStr::CStr> fs_RelevantHelpGlobalOptions();

		void f_AddHelpCommand();
		void f_AddTerminalOptions();
		void f_SetDefaultCommand(CCommand const &_Command);
		void f_SetProgramDescription(NStr::CStr const &_Heading, NStr::CStr const &_Description);
		void f_RegisterGlobalOptions(NEncoding::CEJSON const &_Options);
		CParsedCommandLine f_ParseCommandLine(NContainer::TCVector<NStr::CStr> const &_Params, NCommandLine::EAnsiEncodingFlag _AnsiFlags);

		CInternal &f_AccessInternal();

	private:
		NStorage::TCUniquePointer<CInternal> mp_pInternal;
	};

	extern template struct TCCommandLineSpecification<CCommandLineSpecificationNoCustomization>;
	using CCommandLineSpecification = TCCommandLineSpecification<CCommandLineSpecificationNoCustomization>;

	struct CCommandLineDefaults
	{
		static NCommandLine::EAnsiEncodingFlag fs_ColorAnsiFlagsDefault();
		static bool fs_ColorEnabledDefault();
		static bool fs_Color24BitEnabledDefault();
		static bool fs_ColorLightBackgroundDefault();
		static bool fs_BoxDrawingDefault();
	};
}

#ifndef DMibPNoShortCuts
	using namespace NMib::NCommandLine;
#endif

#include "Malterlib_CommandLine_Console.h"
#include "Malterlib_CommandLine.hpp"
