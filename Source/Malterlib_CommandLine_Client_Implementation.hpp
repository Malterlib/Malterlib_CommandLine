// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

#include <Mib/CommandLine/TableRenderer>

namespace NMib::NCommandLine
{
	template <typename t_CCustomization, typename t_CThis>
	auto TCCommandLineClient<t_CCustomization, t_CThis>::f_ParseCommandLine(NContainer::TCVector<NStr::CStr> &&_Params) -> typename CCommandLineSpecification::CParsedCommandLine
	{
		NException::CDisableExceptionTraceScope DisableTrace;
		return mp_pCommandLineSpecification->f_ParseCommandLine(fg_Move(_Params), f_AnsiEncodingFlags());
	}

	template <typename t_CCustomization, typename t_CThis>
	aint TCCommandLineClient<t_CCustomization, t_CThis>::f_RunCommandLine(NContainer::TCVector<NStr::CStr> &&_CommandLine)
	{
		NException::CDisableExceptionTraceScope DisableTrace;
		auto ParsedCommandLine = mp_pCommandLineSpecification->f_ParseCommandLine(fg_Move(_CommandLine), f_AnsiEncodingFlags());
		return f_RunCommand(fg_Move(ParsedCommandLine.m_Command), fg_Move(ParsedCommandLine.m_Params));
	}

	template <typename t_CCustomization, typename t_CThis>
	bool TCCommandLineClient<t_CCustomization, t_CThis>::f_ColorEnabled() const
	{
		return mp_AnsiFlags & EAnsiEncodingFlag_Color;
	}

	template <typename t_CCustomization, typename t_CThis>
	bool TCCommandLineClient<t_CCustomization, t_CThis>::f_Color24BitEnabled() const
	{
		return mp_AnsiFlags & EAnsiEncodingFlag_Color24Bit;
	}

	template <typename t_CCustomization, typename t_CThis>
	bool TCCommandLineClient<t_CCustomization, t_CThis>::f_ColorLightBackground() const
	{
		return mp_AnsiFlags & EAnsiEncodingFlag_ColorLightBackground;
	}

	template <typename t_CCustomization, typename t_CThis>
	EAnsiEncodingFlag TCCommandLineClient<t_CCustomization, t_CThis>::f_AnsiEncodingFlags() const
	{
		return mp_AnsiFlags;
	}

	template <typename t_CCustomization, typename t_CThis>
	CAnsiEncoding TCCommandLineClient<t_CCustomization, t_CThis>::f_AnsiEncoding() const
	{
		return CAnsiEncoding(mp_AnsiFlags);
	}

	template <typename t_CCustomization, typename t_CThis>
	uint32 TCCommandLineClient<t_CCustomization, t_CThis>::f_CommandLineWidth() const
	{
		return mp_CommandLineWidth;
	}

	template <typename t_CCustomization, typename t_CThis>
	uint32 TCCommandLineClient<t_CCustomization, t_CThis>::f_CommandLineHeight() const
	{
		return mp_CommandLineHeight;
	}

	template <typename t_CCustomization, typename t_CThis>
	CTableRenderHelper TCCommandLineClient<t_CCustomization, t_CThis>::f_TableRenderer(NStr::CStr const &_Prefix) const
	{
		CTableRenderHelper Renderer
			(
				[](NStr::CStr const &_Output)
				{
					DMibConOutRaw(_Output);
				}
				, CTableRenderHelper::EOption_Rounded
				, mp_AnsiFlags
				, mp_CommandLineWidth
			)
		;
		Renderer.f_SetPrefix(_Prefix);
		return Renderer;
	}

	template <typename t_CCustomization, typename t_CThis>
	aint TCCommandLineClient<t_CCustomization, t_CThis>::f_RunCommand(NStr::CStr &&_Command, NEncoding::CEJsonSorted &&_Params)
	{
		using namespace NStr;

		auto &CommandLineSpec = mp_pCommandLineSpecification->f_AccessInternal();

		mp_AnsiFlags = CCommandLineDefaults::fs_ParseAnsiEncodingParams(_Params);

		auto ConsoleProperties = NSys::fg_GetConsoleProperties();

		mp_CommandLineWidth = ConsoleProperties.m_Width;
		mp_CommandLineHeight = ConsoleProperties.m_Height;

		if (auto Value = _Params.f_GetMemberValue("TerminalWidth", -1).f_Integer(); Value > 0)
			mp_CommandLineWidth = Value;
		if (auto Value = _Params.f_GetMemberValue("TerminalHeight", -1).f_Integer(); Value > 0)
			mp_CommandLineHeight = Value;

		enum EHelpCommand
		{
			EHelpCommand_None
			, EHelpCommand_Normal
			, EHelpCommand_Verbose
		};

		auto fCommandHelp = [&]() -> EHelpCommand
			{
				if (auto pValue = _Params.f_GetMember("HelpCurrentCommandVerbose"))
				{
					if (pValue->f_Boolean())
						return EHelpCommand_Verbose;
				}
				if (auto pValue = _Params.f_GetMember("HelpCurrentCommand"))
				{
					if (pValue->f_Boolean())
						return EHelpCommand_Normal;
				}
				return EHelpCommand_None;
			}
		;

		if (auto HelpCommand = fCommandHelp())
		{
			NContainer::TCVector<CStr> Params =
				{
					NFile::CFile::fs_GetFile(NFile::CFile::fs_GetProgramPath())
					, "--help"
					, f_ColorEnabled() ? "--color" : "--no-color"
					, f_Color24BitEnabled() ? "--color-24bit" : "--no-color-24bit"
					, f_ColorLightBackground() ? "--color-light" : "--no-color-light"
					, (mp_AnsiFlags & EAnsiEncodingFlag_BoxDrawing) ? "--box-drawing" : "--no-box-drawing"
					, "--terminal-width={}"_f << _Params.f_GetMemberValue("TerminalWidth", -1).f_Integer()
					, "--terminal-height={}"_f << _Params.f_GetMemberValue("TerminalHeight", -1).f_Integer()
					, _Command
				}
			;

			if (HelpCommand == EHelpCommand_Verbose)
				Params.f_Insert("-v");

			return f_RunCommandLine(fg_Move(Params));
		}

		auto pFoundCommand = CommandLineSpec.m_CommandByName.f_FindEqual(_Command);
		if (!pFoundCommand)
			DMibError(fg_Format("Command not found: {}", pFoundCommand));

		auto &Command = **pFoundCommand;

		return static_cast<CThis *>(this)->fp_RunCommand(&Command, fg_Move(_Params));
	}

	template <typename t_CCustomization, typename t_CThis>
	uint32 TCCommandLineClient<t_CCustomization, t_CThis>::fp_RunCommand
		(
			void const *_pCommand
			, NEncoding::CEJsonSorted &&_Params
		)
	{
		if constexpr (NTraits::cIsSame<CThis, TCCommandLineClient>)
		{
			typename CCommandLineSpecification::CInternal::CCommand const *pCommand = fg_AutoStaticCast(_pCommand);
			if (pCommand->m_pDirectRunCommand)
				return (*pCommand->m_pDirectRunCommand)(fg_Move(_Params), *this);
		}
		return 0;
	}

	template <typename t_CCustomization, typename t_CThis>
	void TCCommandLineClient<t_CCustomization, t_CThis>::f_MutateCommandLineSpecification(NFunction::TCFunction<void (CCommandLineSpecification &o_CommandLine)> const &_fMutate)
	{
		_fMutate(*mp_pCommandLineSpecification);
	}

	template <typename t_CCustomization, typename t_CThis>
	TCCommandLineClient<t_CCustomization, t_CThis>::TCCommandLineClient(NStorage::TCSharedPointer<CCommandLineSpecification> const &_pCommandLineSpecification)
		: mp_pCommandLineSpecification(_pCommandLineSpecification)
		, mp_AnsiFlags(CCommandLineDefaults::fs_ColorAnsiFlagsDefault())
	{
	}

	template <typename t_CCustomization, typename t_CThis>
	TCCommandLineClient<t_CCustomization, t_CThis>::~TCCommandLineClient() = default;

	template <typename t_CCustomization, typename t_CThis>
	TCCommandLineClient<t_CCustomization, t_CThis>::TCCommandLineClient(TCCommandLineClient &&_Other) = default;

	template <typename t_CCustomization, typename t_CThis>
	TCCommandLineClient<t_CCustomization, t_CThis> &TCCommandLineClient<t_CCustomization, t_CThis>::operator =(TCCommandLineClient &&_Other) = default;
}
