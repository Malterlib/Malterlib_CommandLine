// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

namespace NMib::NCommandLine::NPlatform
{
	COnScopeExitShared fg_Process_WaitForScreenChange(NFunction::TCFunction<void (NSys::CConsoleProperties const &_ConsoleProperties)> &&_fOnScreenChange);
}

#ifndef DMibPNoShortCuts
	using namespace NMib::NCommandLine;
#endif
