// Copyright © 2015 Hansoft AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#include "Malterlib_CommandLine.h"
#include "Malterlib_CommandLine_Implementation.h"
#include "Malterlib_CommandLine_Client_Implementation.h"

namespace NMib::NCommandLine
{
	template struct TCCommandLineClient<CCommandLineSpecificationNoCustomization>;
}
