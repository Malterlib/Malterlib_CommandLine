// Copyright © 2015 Hansoft AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#define DMibRuntimeTypeRegistry

#include <Mib/Test/Exception>
#include <Mib/Encoding/JsonShortcuts>
#include <Mib/CommandLine/CommandLine>
#include <Mib/CommandLine/CommandLineClient>
#include <Mib/CommandLine/CommandLineImplementation>

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
					CEJsonSorted RunParams;

					TCSharedPointer<CCommandLineSpecification> pSpecification = fg_Construct();

					auto &Specification = *pSpecification;

					auto Section = Specification.f_AddSection("Test1", "Testing test 2");

					[[maybe_unused]] auto Command = Section.f_RegisterDirectCommand
						(
							{
								"Names"_o= _o["--test", "-t"]
								, "Description"_o= "Test"
								, "Parameters"_o=
								{
									"ArgString?"_o=
									{
										"Default"_o= ""
										, "Description"_o= "Test"
									}
								}
								, "Options"_o=
								{
									"Boolean?"_o=
									{
										"Names"_o= _o["--boolean"]
										, "Default"_o= false
										, "Description"_o= "Test"
									}
									, "Integer?"_o=
									{
										"Names"_o= _o["--integer"]
										, "Default"_o= 5
										, "Description"_o= "Test"
									}
									, "Float?"_o=
									{
										"Names"_o= _o["--float"]
										, "Default"_o= 5.5
										, "Description"_o= "Test"
									}
									, "String?"_o=
									{
										"Names"_o= _o["--string"]
										, "Default"_o= "TestStr"
										, "Description"_o= "Test"
									}
									, "Binary?"_o=
									{
										"Names"_o= _o["--binary"]
										, "Default"_o= fg_CreateVector<uint8>(5, 6, 7)
										, "Description"_o= "Test"
									}
									, "StringChoice?"_o=
									{
										"Names"_o= _o["--string-choice"]
										, "Type"_o= COneOf{"TestStr", "TestStr2"}
										, "Default"_o= "TestStr"
										, "Description"_o= "Test"
									}
									, "Array?"_o=
									{
										"Names"_o= _o["--array"]
										, "Type"_o= _o[]
										, "Default"_o= _o["Test1", "Test2", _o={"Test"_o= 5}]
										, "Description"_o= "Test"
									}
									, "ArraySpecified1"_o=
									{
										"Names"_o= _o["--array-specified1"]
										, "Type"_o= fg_CreateVector<CStr>()
										, "Description"_o= "Test"
									}
									, "ArraySpecified2"_o=
									{
										"Names"_o= _o["--array-specified2"]
										, "Type"_o= fg_CreateVector<CStr>()
										, "Description"_o= "Test"
									}
									, "ArrayChoice?"_o=
									{
										"Names"_o= _o["--array-choice"]
										, "Type"_o= _o[COneOf{"", "Test1", "Test2"}]
										, "Default"_o= _o["Test1", "Test2"]
										, "Description"_o= "Test"
									}
									, "ComplexObject?"_o=
									{
										"Names"_o= _o["--complex-object"]
										, "Type"_o=
										{
											"Key1?"_o= ""
											, "Key2?"_o= _o[""]
											, "Key3?"_o= _o[COneOf{"Test1", "Test2"}]
										}
										, "Default"_o=
										{
											"Key2"_o= _o["Test2"]
										}
										, "Description"_o= "Test 4"
									}
								}
							}
							, [&](CEJsonSorted const &_Params, CCommandLineClient &_CommandLineClient) -> uint32
							{
								RunParams = _Params;
								return 66;
							}
						)
					;

					CCommandLineClient Client(pSpecification);

					DMibTestPath("General");
					aint Ret = Client.f_RunCommandLine(fg_CreateVector<CStr>("App", "--test", "--array-specified1", "[\"Test1\", \"Test2\"]", "--array-specified2", "Test1,Test2"));
					DMibExpect(Ret, ==, 66);
					DMibExpect
						(
							RunParams
							, ==
							,
							(
								_=
								{
									"Boolean"_= false
									, "Integer"_= 5
									, "Float"_= 5.5
									, "String"_= "TestStr"
									, "Binary"_= fg_CreateVector<uint8>(5, 6, 7)
									, "StringChoice"_= "TestStr"
									, "Array"_= _["Test1", "Test2", _={"Test"_= 5}]
									, "ArraySpecified1"_= _["Test1", "Test2"]
									, "ArraySpecified2"_= _["Test1", "Test2"]
									, "ArrayChoice"_= _["Test1", "Test2"]
									, "ComplexObject"_=
									{
										"Key2"_= _["Test2"]
									}
									, "ArgString"_= ""
									, "Command"_= "--test"
								}
							)
						)
					;
				};

				DMibTestSuite("Optional Parameters")
				{
					auto fCallback = [](CEJsonSorted *_pRunParams)
						{
							return [_pRunParams](CEJsonSorted const &_Params, CCommandLineClient &) -> uint32
								{
									*_pRunParams = _Params;
									return 0;
								}
							;
						}
					;

					{
						DMibTestPath("Default used when not provided");
						CEJsonSorted RunParams;
						TCSharedPointer<CCommandLineSpecification> pSpec = fg_Construct();
						auto Section = pSpec->f_AddSection("Test", "Test");
						Section.f_RegisterDirectCommand
							(
								{
									"Names"_o= _o["--cmd"]
									, "Description"_o= "Test"
									, "Parameters"_o=
									{
										"Required"_o=
										{
											"Type"_o= ""
											, "Description"_o= "Required param"
										}
										, "Optional?"_o=
										{
											"Default"_o= "default_val"
											, "Description"_o= "Optional param"
										}
									}
								}
								, fCallback(&RunParams)
							)
						;
						CCommandLineClient Client(pSpec);
						Client.f_RunCommandLine(fg_CreateVector<CStr>("App", "--cmd", "val1"));
						DMibExpect(RunParams["Required"].f_String(), ==, "val1");
						DMibExpect(RunParams["Optional"].f_String(), ==, "default_val");
					};

					{
						DMibTestPath("Optional parameter set when provided");
						CEJsonSorted RunParams;
						TCSharedPointer<CCommandLineSpecification> pSpec = fg_Construct();
						auto Section = pSpec->f_AddSection("Test", "Test");
						Section.f_RegisterDirectCommand
							(
								{
									"Names"_o= _o["--cmd"]
									, "Description"_o= "Test"
									, "Parameters"_o=
									{
										"Required"_o=
										{
											"Type"_o= ""
											, "Description"_o= "Required param"
										}
										, "Optional?"_o=
										{
											"Default"_o= "default_val"
											, "Description"_o= "Optional param"
										}
									}
								}
								, fCallback(&RunParams)
							)
						;
						CCommandLineClient Client(pSpec);
						Client.f_RunCommandLine(fg_CreateVector<CStr>("App", "--cmd", "val1", "val2"));
						DMibExpect(RunParams["Required"].f_String(), ==, "val1");
						DMibExpect(RunParams["Optional"].f_String(), ==, "val2");
					};

					{
						DMibTestPath("Missing required parameter throws");
						TCSharedPointer<CCommandLineSpecification> pSpec = fg_Construct();
						auto Section = pSpec->f_AddSection("Test", "Test");
						Section.f_RegisterDirectCommand
							(
								{
									"Names"_o= _o["--cmd"]
									, "Description"_o= "Test"
									, "Parameters"_o=
									{
										"Required"_o=
										{
											"Type"_o= ""
											, "Description"_o= "Required param"
										}
									}
								}
								, [](CEJsonSorted const &, CCommandLineClient &) -> uint32 { return 0; }
							)
						;
						DMibExpectException
							(
								pSpec->f_ParseCommandLine(fg_CreateVector<CStr>("App", "--cmd"), EAnsiEncodingFlag_None)
								, DMibErrorInstanceExceptionVector("Missing required command parameters: Required\n\nTo see command syntax, run command with '-?'\n", {})
							)
						;
					};

					{
						DMibTestPath("Vector parameter");
						CEJsonSorted RunParams;
						TCSharedPointer<CCommandLineSpecification> pSpec = fg_Construct();
						auto Section = pSpec->f_AddSection("Test", "Test");
						Section.f_RegisterDirectCommand
							(
								{
									"Names"_o= _o["--cmd"]
									, "Description"_o= "Test"
									, "Parameters"_o=
									{
										"Items..."_o=
										{
											"Type"_o= _o[""]
											, "Description"_o= "Vector param"
										}
									}
								}
								, fCallback(&RunParams)
							)
						;
						CCommandLineClient Client(pSpec);
						Client.f_RunCommandLine(fg_CreateVector<CStr>("App", "--cmd", "a", "b", "c"));
						DMibExpect(RunParams["Items"], ==, (_["a", "b", "c"]));
					};
				};

				DMibTestSuite("Optional Options")
				{
					auto fCallback = [](CEJsonSorted *_pRunParams)
						{
							return [_pRunParams](CEJsonSorted const &_Params, CCommandLineClient &) -> uint32
								{
									*_pRunParams = _Params;
									return 0;
								}
							;
						}
					;

					{
						DMibTestPath("Optional option uses default");
						CEJsonSorted RunParams;
						TCSharedPointer<CCommandLineSpecification> pSpec = fg_Construct();
						auto Section = pSpec->f_AddSection("Test", "Test");
						Section.f_RegisterDirectCommand
							(
								{
									"Names"_o= _o["--cmd"]
									, "Description"_o= "Test"
									, "Options"_o=
									{
										"OptValue?"_o=
										{
											"Names"_o= _o["--opt"]
											, "Default"_o= "default_val"
											, "Description"_o= "Optional option"
										}
									}
								}
								, fCallback(&RunParams)
							)
						;
						CCommandLineClient Client(pSpec);
						Client.f_RunCommandLine(fg_CreateVector<CStr>("App", "--cmd"));
						DMibExpect(RunParams["OptValue"].f_String(), ==, "default_val");
					};

					{
						DMibTestPath("Required option must be provided");
						TCSharedPointer<CCommandLineSpecification> pSpec = fg_Construct();
						auto Section = pSpec->f_AddSection("Test", "Test");
						Section.f_RegisterDirectCommand
							(
								{
									"Names"_o= _o["--cmd"]
									, "Description"_o= "Test"
									, "Options"_o=
									{
										"ReqValue"_o=
										{
											"Names"_o= _o["--req"]
											, "Type"_o= ""
											, "Description"_o= "Required option"
										}
									}
								}
								, [](CEJsonSorted const &, CCommandLineClient &) -> uint32 { return 0; }
							)
						;
						DMibExpectException
							(
								pSpec->f_ParseCommandLine(fg_CreateVector<CStr>("App", "--cmd"), EAnsiEncodingFlag_None)
								, DMibErrorInstanceExceptionVector("Missing required option: --req\n\nTo see command syntax, run command with '-?'\n", {})
							)
						;
					};

					{
						DMibTestPath("Required option succeeds when provided");
						CEJsonSorted RunParams;
						TCSharedPointer<CCommandLineSpecification> pSpec = fg_Construct();
						auto Section = pSpec->f_AddSection("Test", "Test");
						Section.f_RegisterDirectCommand
							(
								{
									"Names"_o= _o["--cmd"]
									, "Description"_o= "Test"
									, "Options"_o=
									{
										"ReqValue"_o=
										{
											"Names"_o= _o["--req"]
											, "Type"_o= ""
											, "Description"_o= "Required option"
										}
									}
								}
								, fCallback(&RunParams)
							)
						;
						CCommandLineClient Client(pSpec);
						Client.f_RunCommandLine(fg_CreateVector<CStr>("App", "--cmd", "--req", "val1"));
						DMibExpect(RunParams["ReqValue"].f_String(), ==, "val1");
					};
				};

				DMibTestSuite("Defaults")
				{
					auto fCallback = [](CEJsonSorted *_pRunParams)
						{
							return [_pRunParams](CEJsonSorted const &_Params, CCommandLineClient &) -> uint32
								{
									*_pRunParams = _Params;
									return 0;
								}
							;
						}
					;

					{
						DMibTestPath("Type inferred from default");
						CEJsonSorted RunParams;
						TCSharedPointer<CCommandLineSpecification> pSpec = fg_Construct();
						auto Section = pSpec->f_AddSection("Test", "Test");
						Section.f_RegisterDirectCommand
							(
								{
									"Names"_o= _o["--cmd"]
									, "Description"_o= "Test"
									, "Options"_o=
									{
										"IntOpt?"_o=
										{
											"Names"_o= _o["--int"]
											, "Default"_o= 42
											, "Description"_o= "Integer"
										}
										, "FloatOpt?"_o=
										{
											"Names"_o= _o["--float"]
											, "Default"_o= 3.14
											, "Description"_o= "Float"
										}
										, "BoolOpt?"_o=
										{
											"Names"_o= _o["--bool"]
											, "Default"_o= false
											, "Description"_o= "Boolean"
										}
										, "StrOpt?"_o=
										{
											"Names"_o= _o["--str"]
											, "Default"_o= "hello"
											, "Description"_o= "String"
										}
									}
								}
								, fCallback(&RunParams)
							)
						;
						CCommandLineClient Client(pSpec);
						Client.f_RunCommandLine(fg_CreateVector<CStr>("App", "--cmd"));
						DMibExpect(RunParams["IntOpt"].f_Integer(), ==, 42);
						DMibExpect(RunParams["FloatOpt"].f_Float(), ==, 3.14);
						DMibExpect(RunParams["BoolOpt"].f_Boolean(), ==, false);
						DMibExpect(RunParams["StrOpt"].f_String(), ==, "hello");
					};

					{
						DMibTestPath("Explicit type with no default");
						CEJsonSorted RunParams;
						TCSharedPointer<CCommandLineSpecification> pSpec = fg_Construct();
						auto Section = pSpec->f_AddSection("Test", "Test");
						Section.f_RegisterDirectCommand
							(
								{
									"Names"_o= _o["--cmd"]
									, "Description"_o= "Test"
									, "Options"_o=
									{
										"IntOpt"_o=
										{
											"Names"_o= _o["--int"]
											, "Type"_o= 0
											, "Description"_o= "Integer"
										}
									}
								}
								, fCallback(&RunParams)
							)
						;
						CCommandLineClient Client(pSpec);
						Client.f_RunCommandLine(fg_CreateVector<CStr>("App", "--cmd", "--int", "99"));
						DMibExpect(RunParams["IntOpt"].f_Integer(), ==, 99);
					};

					{
						DMibTestPath("Type override with COneOf");
						CEJsonSorted RunParams;
						TCSharedPointer<CCommandLineSpecification> pSpec = fg_Construct();
						auto Section = pSpec->f_AddSection("Test", "Test");
						Section.f_RegisterDirectCommand
							(
								{
									"Names"_o= _o["--cmd"]
									, "Description"_o= "Test"
									, "Options"_o=
									{
										"ChoiceOpt?"_o=
										{
											"Names"_o= _o["--choice"]
											, "Type"_o= COneOf{"alpha", "beta", "gamma"}
											, "Default"_o= "alpha"
											, "Description"_o= "Choice"
										}
									}
								}
								, fCallback(&RunParams)
							)
						;
						CCommandLineClient Client(pSpec);

						Client.f_RunCommandLine(fg_CreateVector<CStr>("App", "--cmd"));
						DMibExpect(RunParams["ChoiceOpt"].f_String(), ==, "alpha");

						Client.f_RunCommandLine(fg_CreateVector<CStr>("App", "--cmd", "--choice", "beta"));
						DMibExpect(RunParams["ChoiceOpt"].f_String(), ==, "beta");

						DMibExpectException
							(
								pSpec->f_ParseCommandLine(fg_CreateVector<CStr>("App", "--cmd", "--choice", "invalid"), EAnsiEncodingFlag_None)
								, DMibErrorInstanceExceptionVector
								(
									"--choice: Could not match \"invalid\" to any member in set: [\n    \"alpha\",\n    \"beta\",\n    \"gamma\"\n]"
									"\n\nTo see command syntax, run command with '-?'\n"
									, {}
								)
							)
						;
					};
				};

				DMibTestSuite("Global Options")
				{
					CEJsonSorted RunParams;
					TCSharedPointer<CCommandLineSpecification> pSpec = fg_Construct();
					auto Section = pSpec->f_AddSection("Test", "Test");

					pSpec->f_RegisterGlobalOptions
						(
							{
								"Verbose?"_o=
								{
									"Names"_o= _o["--verbose", "-v"]
									, "Default"_o= false
									, "Description"_o= "Verbose output"
								}
							}
						)
					;

					auto fCallback = [&](CEJsonSorted const &_Params, CCommandLineClient &) -> uint32
						{
							RunParams = _Params;
							return 0;
						}
					;

					Section.f_RegisterDirectCommand
						(
							{
								"Names"_o= _o["--cmd1"]
								, "Description"_o= "Command 1"
							}
							, fCallback
						)
					;

					Section.f_RegisterDirectCommand
						(
							{
								"Names"_o= _o["--cmd2"]
								, "Description"_o= "Command 2"
							}
							, fCallback
						)
					;

					CCommandLineClient Client(pSpec);

					{
						DMibTestPath("Available on command 1");
						Client.f_RunCommandLine(fg_CreateVector<CStr>("App", "--cmd1", "--verbose"));
						DMibExpect(RunParams["Verbose"].f_Boolean(), ==, true);
					};

					{
						DMibTestPath("Available on command 2 with short name");
						Client.f_RunCommandLine(fg_CreateVector<CStr>("App", "--cmd2", "-v"));
						DMibExpect(RunParams["Verbose"].f_Boolean(), ==, true);
					};

					{
						DMibTestPath("Default when not provided");
						Client.f_RunCommandLine(fg_CreateVector<CStr>("App", "--cmd1"));
						DMibExpect(RunParams["Verbose"].f_Boolean(), ==, false);
					};
				};

				DMibTestSuite("Section Options")
				{
					CEJsonSorted RunParams;
					TCSharedPointer<CCommandLineSpecification> pSpec = fg_Construct();
					auto Section = pSpec->f_AddSection("Test", "Test");

					Section.f_RegisterSectionOptions
						(
							{
								"Format?"_o=
								{
									"Names"_o= _o["--format"]
									, "Default"_o= "text"
									, "Description"_o= "Output format"
								}
							}
						)
					;

					Section.f_RegisterDirectCommand
						(
							{
								"Names"_o= _o["--cmd"]
								, "Description"_o= "Command"
							}
							, [&](CEJsonSorted const &_Params, CCommandLineClient &) -> uint32
							{
								RunParams = _Params;
								return 0;
							}
						)
					;

					CCommandLineClient Client(pSpec);

					{
						DMibTestPath("Section option default");
						Client.f_RunCommandLine(fg_CreateVector<CStr>("App", "--cmd"));
						DMibExpect(RunParams["Format"].f_String(), ==, "text");
					};

					{
						DMibTestPath("Section option override");
						Client.f_RunCommandLine(fg_CreateVector<CStr>("App", "--cmd", "--format", "json"));
						DMibExpect(RunParams["Format"].f_String(), ==, "json");
					};
				};

				DMibTestSuite("Type Conversions")
				{
					CEJsonSorted RunParams;
					TCSharedPointer<CCommandLineSpecification> pSpec = fg_Construct();
					auto Section = pSpec->f_AddSection("Test", "Test");
					Section.f_RegisterDirectCommand
						(
							{
								"Names"_o= _o["--cmd"]
								, "Description"_o= "Test"
								, "Options"_o=
								{
									"IntOpt?"_o=
									{
										"Names"_o= _o["--int"]
										, "Default"_o= 0
										, "Description"_o= "Integer"
									}
									, "FloatOpt?"_o=
									{
										"Names"_o= _o["--float"]
										, "Default"_o= 0.0
										, "Description"_o= "Float"
									}
									, "BoolOpt?"_o=
									{
										"Names"_o= _o["--bool"]
										, "Default"_o= false
										, "Description"_o= "Boolean"
									}
									, "ArrayOpt?"_o=
									{
										"Names"_o= _o["--array"]
										, "Type"_o= _o[""]
										, "Default"_o= _o[]
										, "Description"_o= "Array"
									}
									, "ObjectOpt?"_o=
									{
										"Names"_o= _o["--object"]
										, "Type"_o=
										{
											"Key1?"_o= ""
											, "Key2?"_o= 0
										}
										, "Default"_o= _o={}
										, "Description"_o= "Object"
									}
								}
							}
							, [&](CEJsonSorted const &_Params, CCommandLineClient &) -> uint32
							{
								RunParams = _Params;
								return 0;
							}
						)
					;
					CCommandLineClient Client(pSpec);

					{
						DMibTestPath("String to integer");
						Client.f_RunCommandLine(fg_CreateVector<CStr>("App", "--cmd", "--int", "42"));
						DMibExpect(RunParams["IntOpt"].f_Integer(), ==, 42);
					};

					{
						DMibTestPath("String to negative integer");
						Client.f_RunCommandLine(fg_CreateVector<CStr>("App", "--cmd", "--int", "-7"));
						DMibExpect(RunParams["IntOpt"].f_Integer(), ==, -7);
					};

					{
						DMibTestPath("String to float");
						Client.f_RunCommandLine(fg_CreateVector<CStr>("App", "--cmd", "--float", "3.14"));
						DMibExpect(RunParams["FloatOpt"].f_Float(), ==, 3.14);
					};

					{
						DMibTestPath("String to boolean true");
						Client.f_RunCommandLine(fg_CreateVector<CStr>("App", "--cmd", "--bool=true"));
						DMibExpect(RunParams["BoolOpt"].f_Boolean(), ==, true);
					};

					{
						DMibTestPath("String to boolean false");
						Client.f_RunCommandLine(fg_CreateVector<CStr>("App", "--cmd", "--bool=false"));
						DMibExpect(RunParams["BoolOpt"].f_Boolean(), ==, false);
					};

					{
						DMibTestPath("Boolean flag without value defaults to true");
						Client.f_RunCommandLine(fg_CreateVector<CStr>("App", "--cmd", "--bool"));
						DMibExpect(RunParams["BoolOpt"].f_Boolean(), ==, true);
					};

					{
						DMibTestPath("JSON array string");
						Client.f_RunCommandLine(fg_CreateVector<CStr>("App", "--cmd", "--array", "[\"a\", \"b\", \"c\"]"));
						DMibExpect(RunParams["ArrayOpt"], ==, (_["a", "b", "c"]));
					};

					{
						DMibTestPath("Comma-separated array");
						Client.f_RunCommandLine(fg_CreateVector<CStr>("App", "--cmd", "--array", "x,y,z"));
						DMibExpect(RunParams["ArrayOpt"], ==, (_["x", "y", "z"]));
					};

					{
						DMibTestPath("Object from JSON string");
						Client.f_RunCommandLine(fg_CreateVector<CStr>("App", "--cmd", "--object", "{\"Key1\": \"val1\", \"Key2\": 5}"));
						DMibExpect(RunParams["ObjectOpt"]["Key1"].f_String(), ==, "val1");
						DMibExpect(RunParams["ObjectOpt"]["Key2"].f_Integer(), ==, 5);
					};

					{
						DMibTestPath("Option with = syntax");
						Client.f_RunCommandLine(fg_CreateVector<CStr>("App", "--cmd", "--int=55"));
						DMibExpect(RunParams["IntOpt"].f_Integer(), ==, 55);
					};

					{
						DMibTestPath("Invalid integer throws");
						DMibExpectException
							(
								pSpec->f_ParseCommandLine(fg_CreateVector<CStr>("App", "--cmd", "--int", "notanumber"), EAnsiEncodingFlag_None)
								, DMibErrorInstanceExceptionVector("--int: Failed to parse \"notanumber\" as a integer value\n\nTo see command syntax, run command with '-?'\n", {})
							)
						;
					};

					{
						DMibTestPath("Invalid float throws");
						DMibExpectException
							(
								pSpec->f_ParseCommandLine(fg_CreateVector<CStr>("App", "--cmd", "--float", "notafloat"), EAnsiEncodingFlag_None)
								, DMibErrorInstanceExceptionVector("--float: Failed to parse \"notafloat\" as a float value\n\nTo see command syntax, run command with '-?'\n", {})
							)
						;
					};
				};

				DMibTestSuite("Boolean Negation")
				{
					CEJsonSorted RunParams;
					TCSharedPointer<CCommandLineSpecification> pSpec = fg_Construct();
					auto Section = pSpec->f_AddSection("Test", "Test");
					Section.f_RegisterDirectCommand
						(
							{
								"Names"_o= _o["--cmd"]
								, "Description"_o= "Test"
								, "Options"_o=
								{
									"Flag?"_o=
									{
										"Names"_o= _o["--flag"]
										, "Default"_o= true
										, "Description"_o= "A flag"
									}
									, "IntOpt?"_o=
									{
										"Names"_o= _o["--int"]
										, "Default"_o= 5
										, "Description"_o= "Integer"
									}
								}
							}
							, [&](CEJsonSorted const &_Params, CCommandLineClient &) -> uint32
							{
								RunParams = _Params;
								return 0;
							}
						)
					;
					CCommandLineClient Client(pSpec);

					{
						DMibTestPath("Negate boolean with --no- prefix");
						Client.f_RunCommandLine(fg_CreateVector<CStr>("App", "--cmd", "--no-flag"));
						DMibExpect(RunParams["Flag"].f_Boolean(), ==, false);
					};

					{
						DMibTestPath("Negate non-boolean throws");
						DMibExpectException
							(
								pSpec->f_ParseCommandLine(fg_CreateVector<CStr>("App", "--cmd", "--no-int"), EAnsiEncodingFlag_None)
								, DMibErrorInstanceExceptionVector("You cannot negate a non-boolean option --int with --no- or +\n\nTo see command syntax, run command with '-?'\n", {})
							)
						;
					};
				};

				DMibTestSuite("Boolean Negation With +")
				{
					CEJsonSorted RunParams;
					TCSharedPointer<CCommandLineSpecification> pSpec = fg_Construct();
					auto Section = pSpec->f_AddSection("Test", "Test");
					Section.f_RegisterDirectCommand
						(
							{
								"Names"_o= _o["--cmd"]
								, "Description"_o= "Test"
								, "Options"_o=
								{
									"FlagOn?"_o=
									{
										"Names"_o= _o["--flag-on", "-f"]
										, "Default"_o= true
										, "Description"_o= "A flag that defaults to on"
									}
									, "FlagOff?"_o=
									{
										"Names"_o= _o["--flag-off", "-o"]
										, "Default"_o= false
										, "Description"_o= "A flag that defaults to off"
									}
									, "FlagOn2?"_o=
									{
										"Names"_o= _o["--flag-on2", "-g"]
										, "Default"_o= true
										, "Description"_o= "Another flag that defaults to on"
									}
									, "IntOpt?"_o=
									{
										"Names"_o= _o["--int", "-i"]
										, "Default"_o= 5
										, "Description"_o= "Integer"
									}
								}
							}
							, [&](CEJsonSorted const &_Params, CCommandLineClient &) -> uint32
							{
								RunParams = _Params;
								return 0;
							}
						)
					;
					CCommandLineClient Client(pSpec);

					{
						DMibTestPath("Negate short boolean with + prefix");
						Client.f_RunCommandLine(fg_CreateVector<CStr>("App", "--cmd", "+f"));
						DMibExpect(RunParams["FlagOn"].f_Boolean(), ==, false);
					};

					{
						DMibTestPath("Negate short boolean with default false using +");
						Client.f_RunCommandLine(fg_CreateVector<CStr>("App", "--cmd", "+o"));
						DMibExpect(RunParams["FlagOff"].f_Boolean(), ==, false);
					};

					{
						DMibTestPath("Grouped + negation");
						Client.f_RunCommandLine(fg_CreateVector<CStr>("App", "--cmd", "+fg"));
						DMibExpect(RunParams["FlagOn"].f_Boolean(), ==, false);
						DMibExpect(RunParams["FlagOn2"].f_Boolean(), ==, false);
					};

					{
						DMibTestPath("Negate non-boolean with + throws");
						DMibExpectException
							(
								pSpec->f_ParseCommandLine(fg_CreateVector<CStr>("App", "--cmd", "+i"), EAnsiEncodingFlag_None)
								, DMibErrorInstanceExceptionVector("You cannot negate a non-boolean option --int with --no- or +\n\nTo see command syntax, run command with '-?'\n", {})
							)
						;
					};
				};

				DMibTestSuite("Registration Errors")
				{
					{
						DMibTestPath("Name cannot start with --no-");
						TCSharedPointer<CCommandLineSpecification> pSpec = fg_Construct();
						auto Section = pSpec->f_AddSection("Test", "Test");
						DMibExpectException
							(
								Section.f_RegisterDirectCommand
								(
									{
										"Names"_o= _o["--no-bad"]
										, "Description"_o= "Test"
									}
									, [](CEJsonSorted const &, CCommandLineClient &) -> uint32 { return 0; }
								)
								, DMibErrorInstance
								(
									"An option or command name cannot start with --no- as this is reserved for negating a boolean option."
									" If this is what you are trying to achieve, rather use then non-negative form and set the default to true."
								)
							)
						;
					};

					{
						DMibTestPath("Name cannot start with +");
						TCSharedPointer<CCommandLineSpecification> pSpec2 = fg_Construct();
						auto Section2 = pSpec2->f_AddSection("Test", "Test");
						DMibExpectException
							(
								Section2.f_RegisterDirectCommand
								(
									{
										"Names"_o= _o["+bad"]
										, "Description"_o= "Test"
									}
									, [](CEJsonSorted const &, CCommandLineClient &) -> uint32 { return 0; }
								)
								, DMibErrorInstance
								(
									"An option or command name cannot start with + as this is reserved for negating a short boolean option."
								)
							)
						;
					};

					{
						DMibTestPath("Name cannot contain =");
						TCSharedPointer<CCommandLineSpecification> pSpec = fg_Construct();
						auto Section = pSpec->f_AddSection("Test", "Test");
						DMibExpectException
							(
								Section.f_RegisterDirectCommand
								(
									{
										"Names"_o= _o["--bad=name"]
										, "Description"_o= "Test"
									}
									, [](CEJsonSorted const &, CCommandLineClient &) -> uint32 { return 0; }
								)
								, DMibErrorInstance("An option or command name cannot contain '=' as this can be used to specify the value for an option.")
							)
						;
					};

					{
						DMibTestPath("Option needs Default or Type");
						TCSharedPointer<CCommandLineSpecification> pSpec = fg_Construct();
						auto Section = pSpec->f_AddSection("Test", "Test");
						DMibExpectException
							(
								Section.f_RegisterDirectCommand
								(
									{
										"Names"_o= _o["--cmd"]
										, "Description"_o= "Test"
										, "Options"_o=
										{
											"BadOpt"_o=
											{
												"Names"_o= _o["--bad"]
												, "Description"_o= "No type or default"
											}
										}
									}
									, [](CEJsonSorted const &, CCommandLineClient &) -> uint32 { return 0; }
								)
								, DMibErrorInstance("An option needs to specify at least 'Default' or 'Type'")
							)
						;
					};

					{
						DMibTestPath("Command needs at least one name");
						TCSharedPointer<CCommandLineSpecification> pSpec = fg_Construct();
						auto Section = pSpec->f_AddSection("Test", "Test");
						DMibExpectException
							(
								Section.f_RegisterDirectCommand
								(
									{
										"Names"_o= _o[]
										, "Description"_o= "Test"
									}
									, [](CEJsonSorted const &, CCommandLineClient &) -> uint32 { return 0; }
								)
								, DMibErrorInstance("You need to specify at least one name for command")
							)
						;
					};

					{
						DMibTestPath("Duplicate command name");
						TCSharedPointer<CCommandLineSpecification> pSpec = fg_Construct();
						auto Section = pSpec->f_AddSection("Test", "Test");
						Section.f_RegisterDirectCommand
							(
								{
									"Names"_o= _o["--cmd"]
									, "Description"_o= "Test"
								}
								, [](CEJsonSorted const &, CCommandLineClient &) -> uint32 { return 0; }
							)
						;
						DMibExpectException
							(
								Section.f_RegisterDirectCommand
								(
									{
										"Names"_o= _o["--cmd"]
										, "Description"_o= "Test 2"
									}
									, [](CEJsonSorted const &, CCommandLineClient &) -> uint32 { return 0; }
								)
								, DMibErrorInstance("Name is already used for another command '--cmd'")
							)
						;
					};

					{
						DMibTestPath("Option identifier conflicts with global option");
						TCSharedPointer<CCommandLineSpecification> pSpec = fg_Construct();
						auto Section = pSpec->f_AddSection("Test", "Test");
						pSpec->f_RegisterGlobalOptions
							(
								{
									"Shared?"_o=
									{
										"Names"_o= _o["--shared-global"]
										, "Default"_o= ""
										, "Description"_o= "Global"
									}
								}
							)
						;
						DMibExpectException
							(
								Section.f_RegisterDirectCommand
								(
									{
										"Names"_o= _o["--cmd"]
										, "Description"_o= "Test"
										, "Options"_o=
										{
											"Shared?"_o=
											{
												"Names"_o= _o["--shared-cmd"]
												, "Default"_o= ""
												, "Description"_o= "Command"
											}
										}
									}
									, [](CEJsonSorted const &, CCommandLineClient &) -> uint32 { return 0; }
								)
								, DMibErrorInstance("Option with same identifier 'Shared' already exists in global options")
							)
						;
					};

					{
						DMibTestPath("Option name conflicts with global option name");
						TCSharedPointer<CCommandLineSpecification> pSpec = fg_Construct();
						auto Section = pSpec->f_AddSection("Test", "Test");
						pSpec->f_RegisterGlobalOptions
							(
								{
									"GlobalOpt?"_o=
									{
										"Names"_o= _o["--same-name"]
										, "Default"_o= ""
										, "Description"_o= "Global"
									}
								}
							)
						;
						DMibExpectException
							(
								Section.f_RegisterDirectCommand
								(
									{
										"Names"_o= _o["--cmd"]
										, "Description"_o= "Test"
										, "Options"_o=
										{
											"CmdOpt?"_o=
											{
												"Names"_o= _o["--same-name"]
												, "Default"_o= ""
												, "Description"_o= "Command"
											}
										}
									}
									, [](CEJsonSorted const &, CCommandLineClient &) -> uint32 { return 0; }
								)
								, DMibErrorInstance("Option with same name '--same-name' already exists in global options")
							)
						;
					};

					{
						DMibTestPath("Option name conflicts with command name");
						TCSharedPointer<CCommandLineSpecification> pSpec = fg_Construct();
						auto Section = pSpec->f_AddSection("Test", "Test");
						Section.f_RegisterDirectCommand
							(
								{
									"Names"_o= _o["--existing-cmd"]
									, "Description"_o= "Existing"
								}
								, [](CEJsonSorted const &, CCommandLineClient &) -> uint32 { return 0; }
							)
						;
						DMibExpectException
							(
								Section.f_RegisterDirectCommand
								(
									{
										"Names"_o= _o["--cmd2"]
										, "Description"_o= "Test"
										, "Options"_o=
										{
											"BadOpt?"_o=
											{
												"Names"_o= _o["--existing-cmd"]
												, "Default"_o= ""
												, "Description"_o= "Conflicts with command"
											}
										}
									}
									, [](CEJsonSorted const &, CCommandLineClient &) -> uint32 { return 0; }
								)
								, DMibErrorInstance("Name is already used as a command '--existing-cmd'")
							)
						;
					};

					{
						DMibTestPath("Command name conflicts with global option name");
						TCSharedPointer<CCommandLineSpecification> pSpec = fg_Construct();
						auto Section = pSpec->f_AddSection("Test", "Test");
						pSpec->f_RegisterGlobalOptions
							(
								{
									"GlobalOpt?"_o=
									{
										"Names"_o= _o["--taken"]
										, "Default"_o= ""
										, "Description"_o= "Global"
									}
								}
							)
						;
						DMibExpectException
							(
								Section.f_RegisterDirectCommand
								(
									{
										"Names"_o= _o["--taken"]
										, "Description"_o= "Test"
									}
									, [](CEJsonSorted const &, CCommandLineClient &) -> uint32 { return 0; }
								)
								, DMibErrorInstance("A global option with same name '--taken' already exists")
							)
						;
					};

					{
						DMibTestPath("Duplicate global option identifier");
						TCSharedPointer<CCommandLineSpecification> pSpec = fg_Construct();
						pSpec->f_RegisterGlobalOptions
							(
								{
									"Dup?"_o=
									{
										"Names"_o= _o["--dup1"]
										, "Default"_o= ""
										, "Description"_o= "First"
									}
								}
							)
						;
						DMibExpectException
							(
								pSpec->f_RegisterGlobalOptions
								(
									{
										"Dup?"_o=
										{
											"Names"_o= _o["--dup2"]
											, "Default"_o= ""
											, "Description"_o= "Second"
										}
									}
								)
								, DMibErrorInstance("A global option with identifier 'Dup' already exists")
							)
						;
					};

					{
						DMibTestPath("Duplicate global option name");
						TCSharedPointer<CCommandLineSpecification> pSpec = fg_Construct();
						pSpec->f_RegisterGlobalOptions
							(
								{
									"Opt1?"_o=
									{
										"Names"_o= _o["--same"]
										, "Default"_o= ""
										, "Description"_o= "First"
									}
								}
							)
						;
						DMibExpectException
							(
								pSpec->f_RegisterGlobalOptions
								(
									{
										"Opt2?"_o=
										{
											"Names"_o= _o["--same"]
											, "Default"_o= ""
											, "Description"_o= "Second"
										}
									}
								)
								, DMibErrorInstance("A global option with same name '--same' already exists")
							)
						;
					};

					{
						DMibTestPath("Global option name conflicts with command");
						TCSharedPointer<CCommandLineSpecification> pSpec = fg_Construct();
						auto Section = pSpec->f_AddSection("Test", "Test");
						Section.f_RegisterDirectCommand
							(
								{
									"Names"_o= _o["--existing"]
									, "Description"_o= "Existing"
								}
								, [](CEJsonSorted const &, CCommandLineClient &) -> uint32 { return 0; }
							)
						;
						DMibExpectException
							(
								pSpec->f_RegisterGlobalOptions
								(
									{
										"BadGlobal?"_o=
										{
											"Names"_o= _o["--existing"]
											, "Default"_o= ""
											, "Description"_o= "Conflicts"
										}
									}
								)
								, DMibErrorInstance("Name is already used as a command '--existing'")
							)
						;
					};

					{
						DMibTestPath("Optional parameter before required parameter");
						TCSharedPointer<CCommandLineSpecification> pSpec = fg_Construct();
						auto Section = pSpec->f_AddSection("Test", "Test");
						DMibExpectException
							(
								Section.f_RegisterDirectCommand
								(
									{
										"Names"_o= _o["--cmd"]
										, "Description"_o= "Test"
										, "Parameters"_o=
										{
											"OptParam?"_o=
											{
												"Default"_o= ""
												, "Description"_o= "Optional"
											}
											, "ReqParam"_o=
											{
												"Type"_o= ""
												, "Description"_o= "Required"
											}
										}
									}
									, [](CEJsonSorted const &, CCommandLineClient &) -> uint32 { return 0; }
								)
								, DMibErrorInstance("Previous parameter was optional, but this one is not. This does not make sense.")
							)
						;
					};

					{
						DMibTestPath("Global option identifier conflicts with command option");
						TCSharedPointer<CCommandLineSpecification> pSpec = fg_Construct();
						auto Section = pSpec->f_AddSection("Test", "Test");
						Section.f_RegisterDirectCommand
							(
								{
									"Names"_o= _o["--cmd"]
									, "Description"_o= "Test"
									, "Options"_o=
									{
										"Conflict?"_o=
										{
											"Names"_o= _o["--conflict-cmd"]
											, "Default"_o= ""
											, "Description"_o= "Command opt"
										}
									}
								}
								, [](CEJsonSorted const &, CCommandLineClient &) -> uint32 { return 0; }
							)
						;
						DMibExpectException
							(
								pSpec->f_RegisterGlobalOptions
								(
									{
										"Conflict?"_o=
										{
											"Names"_o= _o["--conflict-global"]
											, "Default"_o= ""
											, "Description"_o= "Global opt"
										}
									}
								)
								, DMibErrorInstance("Option with same identifier 'Conflict' already exists in command(s): --cmd")
							)
						;
					};

					{
						DMibTestPath("Global option name conflicts with command option name");
						TCSharedPointer<CCommandLineSpecification> pSpec = fg_Construct();
						auto Section = pSpec->f_AddSection("Test", "Test");
						Section.f_RegisterDirectCommand
							(
								{
									"Names"_o= _o["--cmd"]
									, "Description"_o= "Test"
									, "Options"_o=
									{
										"CmdOpt?"_o=
										{
											"Names"_o= _o["--shared-name"]
											, "Default"_o= ""
											, "Description"_o= "Command opt"
										}
									}
								}
								, [](CEJsonSorted const &, CCommandLineClient &) -> uint32 { return 0; }
							)
						;
						DMibExpectException
							(
								pSpec->f_RegisterGlobalOptions
								(
									{
										"GlobalOpt?"_o=
										{
											"Names"_o= _o["--shared-name"]
											, "Default"_o= ""
											, "Description"_o= "Global opt"
										}
									}
								)
								, DMibErrorInstance("Option with name '--shared-name' already exists in command(s): --cmd")
							)
						;
					};

					{
						DMibTestPath("Option identifier conflicts with section option");
						TCSharedPointer<CCommandLineSpecification> pSpec = fg_Construct();
						auto Section = pSpec->f_AddSection("Test", "Test");
						Section.f_RegisterSectionOptions
							(
								{
									"SectOpt?"_o=
									{
										"Names"_o= _o["--sect-opt"]
										, "Default"_o= ""
										, "Description"_o= "Section opt"
									}
								}
							)
						;
						DMibExpectException
							(
								Section.f_RegisterDirectCommand
								(
									{
										"Names"_o= _o["--cmd"]
										, "Description"_o= "Test"
										, "Options"_o=
										{
											"SectOpt?"_o=
											{
												"Names"_o= _o["--sect-opt-cmd"]
												, "Default"_o= ""
												, "Description"_o= "Command opt"
											}
										}
									}
									, [](CEJsonSorted const &, CCommandLineClient &) -> uint32 { return 0; }
								)
								, DMibErrorInstance("Option with same identifier 'SectOpt' already exists in section options")
							)
						;
					};
				};

				DMibTestSuite("Parse Errors")
				{
					{
						DMibTestPath("No command specified");
						TCSharedPointer<CCommandLineSpecification> pSpec = fg_Construct();
						auto Section = pSpec->f_AddSection("Test", "Test");
						Section.f_RegisterDirectCommand
							(
								{
									"Names"_o= _o["--cmd"]
									, "Description"_o= "Test"
								}
								, [](CEJsonSorted const &, CCommandLineClient &) -> uint32 { return 0; }
							)
						;
						DMibExpectException
							(
								pSpec->f_ParseCommandLine(fg_CreateVector<CStr>("App"), EAnsiEncodingFlag_None)
								, DMibErrorInstanceExceptionVector("No command specified\n\nTo list commands, run --help\n", {})
							)
						;
					};

					{
						DMibTestPath("Missing required option");
						TCSharedPointer<CCommandLineSpecification> pSpec = fg_Construct();
						auto Section = pSpec->f_AddSection("Test", "Test");
						Section.f_RegisterDirectCommand
							(
								{
									"Names"_o= _o["--cmd"]
									, "Description"_o= "Test"
									, "Options"_o=
									{
										"Required"_o=
										{
											"Names"_o= _o["--required"]
											, "Type"_o= ""
											, "Description"_o= "Required"
										}
									}
								}
								, [](CEJsonSorted const &, CCommandLineClient &) -> uint32 { return 0; }
							)
						;
						DMibExpectException
							(
								pSpec->f_ParseCommandLine(fg_CreateVector<CStr>("App", "--cmd"), EAnsiEncodingFlag_None)
								, DMibErrorInstanceExceptionVector("Missing required option: --required\n\nTo see command syntax, run command with '-?'\n", {})
							)
						;
					};

					{
						DMibTestPath("Missing required command parameter");
						TCSharedPointer<CCommandLineSpecification> pSpec = fg_Construct();
						auto Section = pSpec->f_AddSection("Test", "Test");
						Section.f_RegisterDirectCommand
							(
								{
									"Names"_o= _o["--cmd"]
									, "Description"_o= "Test"
									, "Parameters"_o=
									{
										"Required"_o=
										{
											"Type"_o= ""
											, "Description"_o= "Required param"
										}
									}
								}
								, [](CEJsonSorted const &, CCommandLineClient &) -> uint32 { return 0; }
							)
						;
						DMibExpectException
							(
								pSpec->f_ParseCommandLine(fg_CreateVector<CStr>("App", "--cmd"), EAnsiEncodingFlag_None)
								, DMibErrorInstanceExceptionVector("Missing required command parameters: Required\n\nTo see command syntax, run command with '-?'\n", {})
							)
						;
					};

					{
						DMibTestPath("Unknown option");
						TCSharedPointer<CCommandLineSpecification> pSpec = fg_Construct();
						auto Section = pSpec->f_AddSection("Test", "Test");
						Section.f_RegisterDirectCommand
							(
								{
									"Names"_o= _o["--cmd"]
									, "Description"_o= "Test"
								}
								, [](CEJsonSorted const &, CCommandLineClient &) -> uint32 { return 0; }
							)
						;
						DMibExpectException
							(
								pSpec->f_ParseCommandLine(fg_CreateVector<CStr>("App", "--cmd", "--unknown"), EAnsiEncodingFlag_None)
								, DMibErrorInstanceExceptionVector
								(
									"No such option or command: --unknown"
									"\n\nTo see command syntax, run command with '-?'\n"
									, {}
								)
							)
						;
					};

					{
						DMibTestPath("Unexpected extra parameter");
						TCSharedPointer<CCommandLineSpecification> pSpec = fg_Construct();
						auto Section = pSpec->f_AddSection("Test", "Test");
						Section.f_RegisterDirectCommand
							(
								{
									"Names"_o= _o["--cmd"]
									, "Description"_o= "Test"
								}
								, [](CEJsonSorted const &, CCommandLineClient &) -> uint32 { return 0; }
							)
						;
						DMibExpectException
							(
								pSpec->f_ParseCommandLine(fg_CreateVector<CStr>("App", "--cmd", "unexpected"), EAnsiEncodingFlag_None)
								, DMibErrorInstanceExceptionVector("Unexpected parameter value: unexpected\n\nTo see command syntax, run command with '-?'\n", {})
							)
						;
					};

					{
						DMibTestPath("Missing option value");
						TCSharedPointer<CCommandLineSpecification> pSpec = fg_Construct();
						auto Section = pSpec->f_AddSection("Test", "Test");
						Section.f_RegisterDirectCommand
							(
								{
									"Names"_o= _o["--cmd"]
									, "Description"_o= "Test"
									, "Options"_o=
									{
										"Value?"_o=
										{
											"Names"_o= _o["--val"]
											, "Default"_o= ""
											, "Description"_o= "Value"
										}
									}
								}
								, [](CEJsonSorted const &, CCommandLineClient &) -> uint32 { return 0; }
							)
						;
						DMibExpectException
							(
								pSpec->f_ParseCommandLine(fg_CreateVector<CStr>("App", "--cmd", "--val"), EAnsiEncodingFlag_None)
								, DMibErrorInstanceExceptionVector("Missing parameter for option: --val\n\nTo see command syntax, run command with '-?'\n", {})
							)
						;
					};

					{
						DMibTestPath("Duplicate command");
						TCSharedPointer<CCommandLineSpecification> pSpec = fg_Construct();
						auto Section = pSpec->f_AddSection("Test", "Test");
						Section.f_RegisterDirectCommand
							(
								{
									"Names"_o= _o["--cmd1"]
									, "Description"_o= "Test 1"
								}
								, [](CEJsonSorted const &, CCommandLineClient &) -> uint32 { return 0; }
							)
						;
						Section.f_RegisterDirectCommand
							(
								{
									"Names"_o= _o["--cmd2"]
									, "Description"_o= "Test 2"
								}
								, [](CEJsonSorted const &, CCommandLineClient &) -> uint32 { return 0; }
							)
						;
						DMibExpectException
							(
								pSpec->f_ParseCommandLine(fg_CreateVector<CStr>("App", "--cmd1", "--cmd2"), EAnsiEncodingFlag_None)
								, DMibErrorInstanceExceptionVector
								(
									"Command --cmd1 already specified. You cannot specify additional command --cmd2\n\nTo see command syntax, run command with '-?'\n"
									, {}
								)
							)
						;
					};

					{
						DMibTestPath("-- separator forces parameters mode");
						CEJsonSorted RunParams;
						TCSharedPointer<CCommandLineSpecification> pSpec = fg_Construct();
						auto Section = pSpec->f_AddSection("Test", "Test");
						Section.f_RegisterDirectCommand
							(
								{
									"Names"_o= _o["--cmd"]
									, "Description"_o= "Test"
									, "Parameters"_o=
									{
										"Param"_o=
										{
											"Type"_o= ""
											, "Description"_o= "Param"
										}
									}
								}
								, [&](CEJsonSorted const &_Params, CCommandLineClient &) -> uint32
								{
									RunParams = _Params;
									return 0;
								}
							)
						;
						CCommandLineClient Client(pSpec);
						Client.f_RunCommandLine(fg_CreateVector<CStr>("App", "--cmd", "--", "--not-an-option"));
						DMibExpect(RunParams["Param"].f_String(), ==, "--not-an-option");
					};

					{
						DMibTestPath("Default command");
						CEJsonSorted RunParams;
						TCSharedPointer<CCommandLineSpecification> pSpec = fg_Construct();
						auto Section = pSpec->f_AddSection("Test", "Test");
						auto DefaultCmd = Section.f_RegisterDirectCommand
							(
								{
									"Names"_o= _o["--default"]
									, "Description"_o= "Default command"
									, "Parameters"_o=
									{
										"Arg?"_o=
										{
											"Default"_o= ""
											, "Description"_o= "Argument"
										}
									}
								}
								, [&](CEJsonSorted const &_Params, CCommandLineClient &) -> uint32
								{
									RunParams = _Params;
									return 42;
								}
							)
						;
						pSpec->f_SetDefaultCommand(DefaultCmd);
						CCommandLineClient Client(pSpec);
						aint Ret = Client.f_RunCommandLine(fg_CreateVector<CStr>("App"));
						DMibExpect(Ret, ==, 42);
						DMibExpect(RunParams["Arg"].f_String(), ==, "");
					};

					{
						CEJsonSorted RunParams;
						TCSharedPointer<CCommandLineSpecification> pSpec = fg_Construct();
						auto Section = pSpec->f_AddSection("Test", "Test");
						Section.f_RegisterDirectCommand
							(
								{
									"Names"_o= _o["--cmd", "-c", "cmd"]
									, "Description"_o= "Multi-alias command"
								}
								, [&](CEJsonSorted const &_Params, CCommandLineClient &) -> uint32
								{
									RunParams = _Params;
									return 0;
								}
							)
						;
						CCommandLineClient Client(pSpec);

						{
							DMibTestPath("Command alias long form");
							Client.f_RunCommandLine(fg_CreateVector<CStr>("App", "--cmd"));
							DMibExpect(RunParams["Command"].f_String(), ==, "--cmd");
						};

						{
							DMibTestPath("Command alias short form");
							Client.f_RunCommandLine(fg_CreateVector<CStr>("App", "-c"));
							DMibExpect(RunParams["Command"].f_String(), ==, "--cmd");
						};

						{
							DMibTestPath("Command alias bare form");
							Client.f_RunCommandLine(fg_CreateVector<CStr>("App", "cmd"));
							DMibExpect(RunParams["Command"].f_String(), ==, "--cmd");
						};
					};
				};

				DMibTestSuite("Validate Command Params")
				{
					TCSharedPointer<CCommandLineSpecification> pSpec = fg_Construct();
					auto Section = pSpec->f_AddSection("Test", "Test");
					Section.f_RegisterDirectCommand
						(
							{
								"Names"_o= _o["--cmd"]
								, "Description"_o= "Test"
								, "Parameters"_o=
								{
									"Arg?"_o=
									{
										"Default"_o= ""
										, "Description"_o= "Arg"
									}
								}
								, "Options"_o=
								{
									"Opt?"_o=
									{
										"Names"_o= _o["--opt"]
										, "Default"_o= "default"
										, "Description"_o= "Option"
									}
								}
							}
							, [](CEJsonSorted const &, CCommandLineClient &) -> uint32 { return 0; }
						)
					;

					auto &Internal = pSpec->f_AccessInternal();
					auto *pCommand = Internal.m_CommandByName.f_FindEqual("--cmd");

					{
						DMibTestPath("Valid params");
						CEJsonSorted Params =
							_=
							{
								"Command"_= "--cmd"
								, "Arg"_= "test"
								, "Opt"_= "value"
							}
						;
						DMibExpectNoException(Internal.f_ValidateParams(**pCommand, Params));
					};

					{
						DMibTestPath("Missing Command key");
						CEJsonSorted Params =
							_=
							{
								"Arg"_= "test"
								, "Opt"_= "default"
							}
						;
						DMibExpectException
							(
								Internal.f_ValidateParams(**pCommand, Params)
								, DMibErrorInstance("'Command' missing in params")
							)
						;
					};

					{
						DMibTestPath("Unexpected parameter key");
						CEJsonSorted Params =
							_=
							{
								"Command"_= "--cmd"
								, "Arg"_= ""
								, "Opt"_= "default"
								, "Unknown"_= "value"
							}
						;
						DMibExpectException
							(
								Internal.f_ValidateParams(**pCommand, Params)
								, DMibErrorInstance("Unexpected parameter 'Unknown' in command params")
							)
						;
					};
				};
			};
		}
	};

	DMibTestRegister(CCommandLine_Tests, Malterlib::CommandLine);
}
