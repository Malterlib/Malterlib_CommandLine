// Copyright © 2015 Hansoft AB 
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#include <Mib/Core/Core>
#include <Mib/Test/Test>

#include <Mib/CommandLine/CommandLine>
#include <Mib/Test/Exception>

using namespace NMib::NContainer;
using namespace NMib::NStr;
using namespace NMib::NTest;
namespace NMib
{
	namespace NCommandLine
	{
		namespace NTests
		{
			class CCmdLine_Tests 
				: public NMib::NTest::CTest
			{

			  public: // Define test suites
				void f_DoTests()
				{
					DMibTestCategory("Use")
					{
						COption Tests("Tests","t","run tests contained in this binary");
						COption ListTests("ListTests","l","list test categories contained in this binary");
						DMibTestSuite("Options")
						{
							COption Unit("Unit","","");
							CParser Parser;
							Parser.f_Add(Tests);
							Parser.f_Add(Unit);

							{
								TCVector<CStr> Argv;
								Argv.f_Insert("App.exe");
								Argv.f_Insert("--Tests");
								auto Args = fg_ParseCommandLine(Parser,Argv);

								DMibTest(DMibExpr(Args.f_IsSet("Tests")));
								DMibTest(DMibExpr(!Args.f_IsSet("Unit")));
							}
                             {
								TCVector<CStr> Argv;
								Argv.f_Insert("App.exe");
								Argv.f_Insert("-t");
								auto Args2 = fg_ParseCommandLine(Parser,Argv);

								DMibTest(DMibExpr(Args2.f_IsSet("Tests")));
								DMibTest(DMibExpr(!Args2.f_IsSet("Unit")));
							}                                                                                        
							{
								auto fl_ThrowsException 
									= [&]
									{
										TCVector<CStr> Argv;
										Argv.f_Insert("App.exe");
										Argv.f_Insert("-x");
										auto Args = fg_ParseCommandLine(Parser,Argv);
									}
								;
								DMibTest(DMibExpr(TCThrowsException<CInvalidArguments>()) == DMibLExpr(fl_ThrowsException();));
							}
							{
								auto fl_ThrowsException2
									= [&]
									{
										TCVector<CStr> Argv;
										Argv.f_Insert("App.exe");
										Argv.f_Insert("-t");
										Argv.f_Insert("Malterlib/Container/*");
										auto Args = fg_ParseCommandLine(Parser,Argv);
									}
								;
								DMibTest(DMibExpr(TCThrowsException<CInvalidArguments>()) == DMibLExpr(fl_ThrowsException2();));
							}
						};
						DMibTestSuite("Enum parameters")
						{
							CParser Parser;
							CValue::CValidValues Valid;
							Valid.f_Insert("Hello");
							Valid.f_Insert("World");
							auto UnitTest2 = Tests;
							UnitTest2.f_AddList(CValue("Greeting",Valid));
							Parser.f_Add(UnitTest2);
							Parser.f_Add(ListTests);

							{
								TCVector<CStr> Argv;
								Argv.f_Insert("App.exe");
								Argv.f_Insert("--Tests");
								Argv.f_Insert("--ListTests");
								auto Args = fg_ParseCommandLine(Parser,Argv);

								DMibTest(DMibExpr(Args.f_IsSet("Tests")));
								DMibTest(DMibExpr(Args.f_IsSet("ListTests")));
							}
							{
								auto fl_ThrowsException
									= [&]
									{
										TCVector<CStr> Argv;
										Argv.f_Insert("App.exe");
										Argv.f_Insert("-t");
										Argv.f_Insert("Malterlib");
										auto Args = fg_ParseCommandLine(Parser,Argv);
									} 
								;
								DMibTest(DMibExpr(TCThrowsException<CInvalidArguments>()) == DMibLExpr(fl_ThrowsException();));
							}
							{
								TCVector<CStr> Argv;
								Argv.f_Insert("App.exe");
								Argv.f_Insert("--Tests");
								Argv.f_Insert("Hello");
								Argv.f_Insert("--ListTests");
								auto Args2 = fg_ParseCommandLine(Parser,Argv);

								DMibTest(DMibExpr(Args2.f_IsSet("Tests")));
								DMibTest(DMibExpr(Args2.f_IsSet("ListTests")));

								auto Value2 = Args2["Tests"];

								DMibTest(DMibExpr(Value2.f_GetLen()) == DMibExpr(1u));
								//DMibTest(DMibExpr(Value[0]));
							}
						};
						DMibTestSuite("Enum list parameters")
						{
							CParser Parser;
							CValue::CValidValues Valid;
							Valid.f_Insert("Hello");
							Valid.f_Insert("World");
							auto UnitTest2 = Tests;
							UnitTest2.f_AddList(CValue("Greeting",Valid));
							Parser.f_Add(UnitTest2);
							Parser.f_Add(ListTests);

							{
								TCVector<CStr> Argv;
								Argv.f_Insert("App.exe");
								Argv.f_Insert("--Tests");
								Argv.f_Insert("--ListTests");
								auto Args = fg_ParseCommandLine(Parser,Argv);

								DMibTest(DMibExpr(Args.f_IsSet("Tests")));
								DMibTest(DMibExpr(Args.f_IsSet("ListTests")));
							}
							{
								auto fl_ThrowsException 
									= [&]
									{
										TCVector<CStr> Argv;
										Argv.f_Insert("App.exe");
										Argv.f_Insert("-t");
										Argv.f_Insert("Malterlib");
										auto Args = fg_ParseCommandLine(Parser,Argv);
									}
								;
								DMibTest(DMibExpr(TCThrowsException<CInvalidArguments>()) == DMibLExpr(fl_ThrowsException();));
							}
							{
								TCVector<CStr> Argv;
								Argv.f_Insert("App.exe");
								Argv.f_Insert("--Tests");
								Argv.f_Insert("(Hello)");
								Argv.f_Insert("--ListTests");
								auto Args2 = fg_ParseCommandLine(Parser,Argv);

								DMibTest(DMibExpr(Args2.f_IsSet("Tests")));
								DMibTest(DMibExpr(Args2.f_IsSet("ListTests")));

								auto Value2 = Args2["Tests"];

								DMibTest(DMibExpr(Value2.f_GetLen()) == DMibExpr(1u));

								DMibTest(DMibExpr(Args2.f_HasList("Tests","Greeting")));
								auto List2 = Args2.f_GetList("Tests","Greeting");
								DMibTest(DMibExpr(List2.f_GetLen()) == DMibExpr(1u));
							}
							{
								TCVector<CStr> Argv;
								Argv.f_Insert("App.exe");
								Argv.f_Insert("--Tests");
								Argv.f_Insert("(Hello");
								Argv.f_Insert("World)");
								Argv.f_Insert("--ListTests");
								auto Args3 = fg_ParseCommandLine(Parser,Argv);

								DMibTest(DMibExpr(Args3.f_IsSet("Tests")));
								DMibTest(DMibExpr(Args3.f_IsSet("ListTests")));

								auto Value3 = Args3["Tests"];

								DMibTest(DMibExpr(Value3.f_GetLen()) == DMibExpr(1u));

								DMibTest(DMibExpr(Args3.f_HasList("Tests","Greeting")));
								auto List3 = Args3.f_GetList("Tests","Greeting");
								DMibTest(DMibExpr(List3.f_GetLen()) == DMibExpr(2u));
							}
							{
								TCVector<CStr> Argv;
								Argv.f_Insert("App.exe");
								Argv.f_Insert("--Tests");
								Argv.f_Insert("(");
								Argv.f_Insert("Hello");
								Argv.f_Insert("World");
								Argv.f_Insert(")");
								Argv.f_Insert("--ListTests");
								auto Args4 = fg_ParseCommandLine(Parser,Argv);

								DMibTest(DMibExpr(Args4.f_IsSet("Tests")));
								DMibTest(DMibExpr(Args4.f_IsSet("ListTests")));

								auto Value4 = Args4["Tests"];

								DMibTest(DMibExpr(Value4.f_GetLen()) == DMibExpr(1u));

								DMibTest(DMibExpr(Args4.f_HasList("Tests","Greeting")));
								auto List4 = Args4.f_GetList("Tests","Greeting");
								DMibTest(DMibExpr(List4.f_GetLen()) == DMibExpr(2u));
							}
							{
								auto fl_ThrowsException2 
									= [&]
									{
										TCVector<CStr> Argv;
										Argv.f_Insert("App.exe");
										Argv.f_Insert("--Tests");
										Argv.f_Insert("(");
										Argv.f_Insert("Hello");
										Argv.f_Insert("Error");
										Argv.f_Insert("World");
										Argv.f_Insert(")");
										Argv.f_Insert("--ListTests");
										auto Args = fg_ParseCommandLine(Parser,Argv);
									}
								;
								DMibTest(DMibExpr(TCThrowsException<CInvalidArguments>()) == DMibLExpr(fl_ThrowsException2();));
							}
						};
						DMibTestSuite("Value parameters")
						{
							CParser Parser;
							Tests.f_Add(CValue("Path"));
							Parser.f_Add(Tests);
							Parser.f_Add(ListTests);

							{
								TCVector<CStr> Argv;
								Argv.f_Insert("App.exe");
								Argv.f_Insert("--Tests");
								Argv.f_Insert("--ListTests");
								auto Args = fg_ParseCommandLine(Parser,Argv);

								DMibTest(DMibExpr(Args.f_IsSet("Tests")));
								DMibTest(DMibExpr(Args.f_IsSet("ListTests")));
							}
							{
								TCVector<CStr> Argv;
								Argv.f_Insert("App.exe");
								Argv.f_Insert("--Tests");
								Argv.f_Insert("Malterlib/Container/*");
								Argv.f_Insert("--ListTests");
								auto Args2 = fg_ParseCommandLine(Parser,Argv);

								DMibTest(DMibExpr(Args2.f_IsSet("Tests")));
								DMibTest(DMibExpr(Args2.f_IsSet("ListTests")));

								auto Value2 = Args2["Tests"];

								DMibTest(DMibExpr(Value2.f_GetLen()) == DMibExpr(1u));
								//DMibTest(DMibExpr(Value[0]));
							}
						};
					};
				}
			};
			DMibTestRegister(CCmdLine_Tests, Malterlib::CommandLine);
		}
	}
}
