// Copyright © 2019 Nonna Holding AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

#include <Mib/Encoding/EJSON>
#include "Malterlib_CommandLine.h"

namespace NMib::NCommandLine
{
	struct CTableRenderHelper;

	template <typename t_CCustomization, typename t_CThis>
	struct TCCommandLineClient
	{
		using CThis = typename TCChooseType<NTraits::TCIsVoid<t_CThis>::mc_Value, TCCommandLineClient, t_CThis>::CType;
		using CCommandLineSpecification = TCCommandLineSpecification<t_CCustomization>;

		typename CCommandLineSpecification::CParsedCommandLine f_ParseCommandLine(NContainer::TCVector<NStr::CStr> const &_Params = fg_GetSys()->f_GetCommandLineArgs());
		aint f_RunCommandLine(NContainer::TCVector<NStr::CStr> const &_CommandLine = fg_GetSys()->f_GetCommandLineArgs());
		aint f_RunCommand(NStr::CStr const &_Command, NEncoding::CEJSONSorted const &_Params);
		void f_MutateCommandLineSpecification(NFunction::TCFunction<void (CCommandLineSpecification &o_CommandLine)> const &_fMutate);

		bool f_ColorEnabled() const;
		bool f_Color24BitEnabled() const;
		bool f_ColorLightBackground() const;

		EAnsiEncodingFlag f_AnsiEncodingFlags() const;
		CAnsiEncoding f_AnsiEncoding() const;
		uint32 f_CommandLineWidth() const;
		uint32 f_CommandLineHeight() const;

		CTableRenderHelper f_TableRenderer(NStr::CStr const &_Prefix = {}) const;

		TCCommandLineClient(NStorage::TCSharedPointer<CCommandLineSpecification> const &_pCommandLineSpecification);
		~TCCommandLineClient();

		TCCommandLineClient(TCCommandLineClient const &_Other) = delete;
		TCCommandLineClient(TCCommandLineClient &&_Other);
		TCCommandLineClient &operator =(TCCommandLineClient const &_Other) = delete;
		TCCommandLineClient &operator =(TCCommandLineClient &&_Other);

	protected:
		struct CInternal;

		uint32 fp_RunCommand(void const *_pCommand, NEncoding::CEJSONSorted const &_Params);

		NStorage::TCSharedPointer<CCommandLineSpecification> mp_pCommandLineSpecification;
		EAnsiEncodingFlag mp_AnsiFlags = EAnsiEncodingFlag_None;
		uint32 mp_CommandLineWidth = 0;
		uint32 mp_CommandLineHeight = 0;
	};

	using CCommandLineClient = TCCommandLineClient<CCommandLineSpecificationNoCustomization>;

	extern template struct TCCommandLineClient<CCommandLineSpecificationNoCustomization>;
}

#ifndef DMibPNoShortCuts
	using namespace NMib::NCommandLine;
#endif
