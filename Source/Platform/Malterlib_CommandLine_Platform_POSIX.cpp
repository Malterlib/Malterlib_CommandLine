// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

using namespace NMib;

#include "../Malterlib_CommandLine_Platform.h"

#include <signal.h>

// Resize callbacks run on the signal subsystem's I/O loop, chosen at its first registration.
// The returned scope owns this callback's registration.
NMib::COnScopeExitShared NMib::NCommandLine::NPlatform::fg_Process_WaitForScreenChange(NFunction::TCFunction<void (NSys::CConsoleProperties const &_ConsoleProperties)> &&_fOnScreenChange)
{
	return NSys::fg_System_RegisterForSignal
		(
			SIGWINCH
			, [fOnScreenChange = fg_Move(_fOnScreenChange)]
			{
				fOnScreenChange(NSys::fg_GetConsoleProperties());
			}
		)
	;
}
