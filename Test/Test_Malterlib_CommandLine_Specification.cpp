// Copyright © 2015 Hansoft AB 
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#define DMibRuntimeTypeRegistry

#include <Mib/Test/Exception>
#include <Mib/Encoding/JSONShortcuts>
#include <Mib/CommandLine/CommandLine>
#include <Mib/CommandLine/CommandLineClient>

using namespace NMib;
using namespace NMib::NContainer;
using namespace NMib::NEncoding;
using namespace NMib::NStr;
using namespace NMib::NFunction;
using namespace NMib::NCommandLine;
using namespace NMib::NStorage;

namespace
{
	class CCommandLine_Tests : public NMib::NTest::CTest
	{
	public:
		void f_DoTests()
		{
			DMibTestCategory("Command Line")
			{
				DMibTestSuite("General")
				{
					CEJSONSorted RunParams;

					TCSharedPointer<CCommandLineSpecification> pSpecification = fg_Construct();

					auto &Specification = *pSpecification;

					auto Section = Specification.f_AddSection("Test1", "Testing test 2");

					[[maybe_unused]] auto Command = Section.f_RegisterDirectCommand
						(
							{
								"Names"_o= {"--test", "-t"}
								, "Description"_o= "Test 3."
								, "Parameters"_o=
								{
									"ArgString?"_o=
									{
										"Default"_o= ""
										, "Description"_o= "Test 5"
									}
								}
								, "Options"_o=
								{
									"Boolean?"_o=
									{
										"Names"_o= {"--boolean"}
										, "Default"_o= false
										, "Description"_o= "Test 1"
									}
									, "Integer?"_o=
									{
										"Names"_o= {"--integer"}
										, "Default"_o= 5
										, "Description"_o= "Test 2"
									}
									, "Float?"_o=
									{
										"Names"_o= {"--float"}
										, "Default"_o= 5.5
										, "Description"_o= "Test 3"
									}
									, "String?"_o=
									{
										"Names"_o= {"--string"}
										, "Default"_o= "TestStr"
										, "Description"_o= "Test 4"
									}
									, "Binary?"_o=
									{
										"Names"_o= {"--binary"}
										, "Default"_o= fg_CreateVector<uint8>(5, 6, 7)
										, "Description"_o= "Test 4"
									}
									, "StringChoice?"_o=
									{
										"Names"_o= {"--string-choice"}
										, "Type"_o= COneOf{"TestStr", "TestStr2"}
										, "Default"_o= "TestStr"
										, "Description"_o= "Test 4"
									}
									, "Array?"_o=
									{
										"Names"_o= {"--array"}
										, "Type"_o= _[_]
										, "Default"_o= {"Test1", "Test2", {"Test"_o= 5}}
										, "Description"_o= "Test 5"
									}
									, "ArrayChoice?"_o=
									{
										"Names"_o= {"--array-choice"}
										, "Type"_o= {COneOf{"", "Test1", "Test2"}}
										, "Default"_o= {"Test1", "Test2"}
										, "Description"_o= "Test 5"
									}
									, "ComplexObject?"_o=
									{
										"Names"_o= {"--complex-object"}
										, "Type"_o=
										{
											"Key1?"_o= ""
											, "Key2?"_o= {""}
											, "Key3?"_o= {COneOf{"Test1", "Test2"}}
										}
										, "Default"_o=
										{
											"Key2"_o= {"Test2"}
										}
										, "Description"_o= "Test 4"
									}
								}
							}
							, [&](CEJSONSorted const &_Params, CCommandLineClient &_CommandLineClient) -> uint32
							{
								RunParams = _Params;
								return 66;
							}
						)
					;

					CCommandLineClient Client(pSpecification);

					DMibTestPath("General");
					aint Ret = Client.f_RunCommandLine(fg_CreateVector<CStr>("App", "--test"));
					DMibExpect(Ret, ==, 66);
					DMibExpect
						(
							RunParams
							, ==
							, CEJSONSorted
							(
								{
									"Boolean"_= false
									, "Integer"_= 5
									, "Float"_= 5.5
									, "String"_= "TestStr"
									, "Binary"_= fg_CreateVector<uint8>(5, 6, 7)
									, "StringChoice"_= "TestStr"
									, "Array"_= {"Test1", "Test2", {"Test"_= 5}}
									, "ArrayChoice"_= {"Test1", "Test2"}
									, "ComplexObject"_= {"Key2"_= {"Test2"}}
									, "ArgString"_= ""
									, "Command"_= "--test"
								}
							)
						)
					;
				};
			};
		}
		
		/* TODO
		Optional
			Parameters
			Options
		Defaults
			Type from defaults
			Type when no default
			Different types and defaults
		Global options
		Local options
		Type conversions
		Register command
			Direct
			Actor
			
		Validate Command Params
		Errors
			DMibError("An optional option needs to specify the 'Default' value");
			DMibError("An option needs to specify at least 'Default' or 'Type'");
			DMibError
				(
					"An option or command name cannot start with --no- as this is reserved for negating a boolean option."
					" If this is what you are trying to achieve, rather use then non-negative form and set the default to false."
				)
			;
			DMibError("An option or command name cannot contain '=' as this can be used to specify the value for an option.");
			Register options
				DMibError(fg_Format("Option with same identifier '{}' already exists in global options", Identifier));
				DMibError(fg_Format("Option with identifier '{}' already exists on command", Identifier));
				DMibError("You need to specify at least one name for option");
				DMibError(fg_Format("Name is already used as a command '{}'", Name));
				DMibError(fg_Format("Option with same name '{}' already exists in global options", Name));
				DMibError(fg_Format("Option with name '{}' already exists on command", Name));
			Register global options
				DMibError(fg_Format("A global option with identifier '{}' already exists", Identifier));
				DMibError(fg_Format("Option with same identifier '{}' already exists in command(s): {}", Identifier, fg_GetFormattedIdentifiers(*pIdentifiers)));
				DMibError("You need to specify at least one name for option");
				DMibError(fg_Format("Name is already used as a command '{}'", Name));
				DMibError(fg_Format("A global option with same name '{}' already exists", Name));
				DMibError(fg_Format("Option with name '{}' already exists in command(s): {}", Name, fg_GetFormattedIdentifiers(*pIdentifiers)));
			Register command
				DMibError("You need to specify at least one name for command");
				DMibError(fg_Format("A global option with same name '{}' already exists", Name));
				DMibError(fg_Format("Option with name '{}' already exists in command(s) so cannot be used as a command name: {}", Name, fg_GetFormattedIdentifiers(*pIdentifiers)));
				DMibError(fg_Format("Name is already used for another command '{}'", Name));
				DMibError("Previous parameter was optional, but this one is not. This does not make sense.");
				DMibError(fg_Format("Duplicate parameter identifier '{}'", Identifier));
		 RunCommand
			DMibError(fg_Format("You cannot specify a command parameter with '=': {}", OptionValue));
			DMibError(fg_Format("Command '{}' already specified. You cannot specify additional command '{}'.", FoundCommandName, ParsedParameter));
			DMibError(fg_Format("You cannot negate a non-boolean option ({}) with --no-", _Value.m_Names.f_GetFirst()));
			DMibError(fg_Format("No such option '{}'", ParsedParameter));
			DMibError(fg_Format("Unexpected parameter: {}", Parameter));
			DMibError(fg_Format("Missing parameter for option: {}", pCurrentOption->m_Names.f_GetFirst()));
			DMibError(fg_Format("Missing required command parameter: {}", iCommandParameter->m_Identifier));
			DMibError("No command specified")		 
			DMibError(fg_Format("Missing required option: {}", _Option.m_Names.f_GetFirst()));
		
		*/ 
		
	};

	DMibTestRegister(CCommandLine_Tests, Malterlib::CommandLine);
}
