// Copyright © 2015 Hansoft AB 
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

#include <Mib/Storage/Variant>

/*
   void main(argv,argc)
   {
   NContainer::TCVector<NStr::CStr> CmdArgs;

   CmdParser cp;

   cp.ProgramName("My app");
   cp.ExeName("My app");
   cp.ProgramDescription("hfaghföadhs");

   struct Cmd;
   struct Option;
   Cmd hlp("help","display help on commands");
   hlp.AddOptional(Cmd("Topic"));
   hlp.AddMandatory(Cmd("Topic"));
   cp.Add(hlp);
   cp.Add(Option("h","help","display help on commands"));

   cp.AddCmd("h","help","display help on commands", );

   value list(unique or not) set/enum  hierarchical commands

   short summary for help:
   p4 add [ -c changelist# ] [ -d -f -n ] [ -t filetype ] file

   ;
   App -e- 
   App -e+
   App -e

    App -Tests (Filter1 Filer2 ...)
	App -Tests "sdf a"

	auto x = parse(CmdArgs, cp);

    if(x.HasOption(bla))
}
*/

namespace NMib::NCommandLine
{
	class CInvalidArguments
	{
	public:
		CInvalidArguments(NStr::CStr const &_Arg)
			: mp_Arg(_Arg)
		{
		}
		NStr::CStr const &f_Arg() const
		{
			return mp_Arg;
		}
	private:
		NStr::CStr mp_Arg;
	};

	class CValue
	{
	public: // types
		typedef NContainer::TCVector<NStr::CStr> CValidValues;
	public:
		CValue(NStr::CStr const &_Name)
			: mp_ValidValues()
			, mp_Name(_Name)
		{
		}
		CValue(NStr::CStr const &_Name, CValidValues _ValidValues)
			: mp_ValidValues(_ValidValues)
			, mp_Name(_Name)
		{
		}
		bint f_IsMatch(NStr::CStr const &_Value) const;
		NStr::CStr const &f_Name() const
		{
			return mp_Name;
		}
	private:
		CValidValues mp_ValidValues;
		NStr::CStr mp_Name;
	};
	class COption
	{
	public:
		COption();;
		COption(NStr::CStr const &_Name, NStr::CStr const &_Short, NStr::CStr const &_Description);
		void f_Add(CValue const &_Value);
		void f_AddList(CValue const &_Value);
		NStr::CStr const &f_Name() const;
		NStr::CStr const &f_ShortName() const;
		bint f_HasShortName() const;
		NStr::CStr const &f_Desc() const;
		typedef NContainer::TCVariant<NStr::CStr, NContainer::TCVector<NStr::CStr>> CArgValue;
	private:
		struct CNamedValue
		{
			CNamedValue( NStr::CStr const &_Name, CArgValue const &_Value)
				: m_Name(_Name)
				, m_Value(_Value)
			{
			}
			NStr::CStr m_Name;
			CArgValue m_Value;
		};
	public:
		NContainer::TCMap<NStr::CStr, CArgValue> f_ReadValues(NContainer::TCVector<NStr::CStr>::CIteratorConst &_Iter) const;

	private:
		struct CArgument
		{
			CValue m_Value;
			bint m_IsList;
			CArgument(CValue const &_Value, bint _IsList)
				: m_Value(_Value)
				, m_IsList(_IsList)

			{
			}
		};
		NStr::CStr mp_Name;
		NStr::CStr mp_ShortName;
		NStr::CStr mp_Description;
		NContainer::TCVector<CArgument> mp_Values;
		//NContainer::TCVector<CEnum> mp_EnumList;
	};
	class CParser
	{
	public:
		CParser();
		void f_Add(COption const &_Option);
		NStr::CStr f_HelpMessage() const;
		bint f_HasOption(NStr::CStr const &_Name) const;
		COption const &f_GetOption(NStr::CStr const &_Name) const;
	private:
		NContainer::TCLinkedList<COption> mp_Options;
		NContainer::TCMap<NStr::CStr, COption const*> mp_LongNames;
		NContainer::TCMap<NStr::CStr, COption const*> mp_ShortNames;
	};
	class CCommandArguments
	{
	public:
		typedef COption::CArgValue CValue;
		typedef NContainer::TCMap<NStr::CStr, NContainer::TCMap<NStr::CStr, CValue>> COptionMap;
	public:
		CCommandArguments();
		CCommandArguments(COptionMap&& _Values);
		bint f_IsSet(NStr::CStr const &_Name) const;
		NContainer::TCMap<NStr::CStr, CValue> const &operator [] (NStr::CStr const &_Name) const;
		bint f_HasValue(NStr::CStr const &_Option, NStr::CStr const &_Value) const;
		NStr::CStr f_GetValue(NStr::CStr const &_Option, NStr::CStr const &_Value) const;
		bint f_HasList(NStr::CStr const &_Option, NStr::CStr const &_Value) const;
		NContainer::TCVector<NStr::CStr> f_GetList(NStr::CStr const &_Option, NStr::CStr const &_Value) const;
		bint f_HasListItem(NStr::CStr const &_Option, NStr::CStr const &_List, NStr::CStr const &_Item) const;
	private:
		COptionMap mp_Values;
	};

	CCommandArguments fg_ParseCommandLine(CParser const &_Parser, NContainer::TCVector<NStr::CStr> const &_Argv);
	CCommandArguments fg_ParseCommandLine(CParser const &_Parser);
}

#ifndef DMibPNoShortCuts
	using namespace NMib::NCommandLine;
#endif

