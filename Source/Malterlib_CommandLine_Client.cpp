// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "Malterlib_CommandLine.h"
#include "Malterlib_CommandLine_Implementation.h"
#include "Malterlib_CommandLine_Client_Implementation.h"

namespace NMib::NCommandLine
{
	template struct TCCommandLineClient<CCommandLineSpecificationNoCustomization>;
}
