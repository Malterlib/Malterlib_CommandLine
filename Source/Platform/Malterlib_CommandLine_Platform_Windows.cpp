// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

using namespace NMib;

#include "../Malterlib_CommandLine_Platform.h"

#include <Mib/Process/StdIn>

// Requires an open console stdin reader; callbacks run on that reader's I/O-loop thread.
// Resize records share the input queue, so only the reader may consume them.
NMib::COnScopeExitShared NMib::NCommandLine::NPlatform::fg_Process_WaitForScreenChange(NFunction::TCFunction<void (NSys::CConsoleProperties const &_ConsoleProperties)> &&_fOnScreenChange)
{
	return NProcess::fg_StdInReader_RegisterScreenChange(fg_Move(_fOnScreenChange));
}
