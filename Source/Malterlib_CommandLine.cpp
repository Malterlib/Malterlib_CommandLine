// Copyright © 2015 Hansoft AB 
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#include "Malterlib_CommandLine.h"

namespace NMib::NCommandLine
{
	NContainer::TCMap<NStr::CStr,COption::CArgValue> COption::f_ReadValues( NContainer::TCVector<NStr::CStr>::CIteratorConst &_Iter ) const
	{
		NContainer::TCMap<NStr::CStr,CArgValue>  Ret;
		auto ArgIt = mp_Values.f_GetIterator();
		auto fl_ReadValue = [&]() -> CNamedValue
		{
			NStr::CStr const &Value = *_Iter;
			//for (auto It = m_Values.f_GetIterator();It;++It)
			{
				if (ArgIt->m_Value.f_IsMatch(Value))
				{
					++_Iter;
					return COption::CNamedValue(ArgIt->m_Value.f_Name(), CArgValue(Value));
				}
			}
			throw CInvalidArguments(Value);
		};
		auto fl_ReadList = [&]() -> CNamedValue
		{
			DMibRequire(!!ArgIt);
			DMibRequire(!!_Iter);
			NContainer::TCVector<NStr::CStr> Ret;
			if ((*_Iter)[0] == '(')
			{
				auto const &Enum = ArgIt->m_Value;
				for (;_Iter;++_Iter)
				{
					auto Value = (*_Iter).f_Replace("(","");
					Value = Value.f_Replace(")","");
					if (Value.f_IsEmpty())
						continue;
					if (Enum.f_IsMatch(Value))
						Ret.f_Insert(Value);
					else
						throw CInvalidArguments(Value);
					if ((*_Iter)[_Iter->f_GetLen()-1] == ')')
					{
						++_Iter;
						break;
					}
				}
			}
			else
			{
				auto const &Enum = ArgIt->m_Value;
				for (;_Iter;++_Iter)
				{
					auto Value = *_Iter;
					if (Enum.f_IsMatch(Value))
						Ret.f_Insert(Value);
					else
						throw CInvalidArguments(Value);
				}
			}
			return COption::CNamedValue(ArgIt->m_Value.f_Name(), CArgValue(fg_Move(Ret)));
		};
		for (;ArgIt;++ArgIt)
		{
			if (!_Iter)
				break;  // no more input
			if (ArgIt->m_IsList)
			{
				auto NamedValue = fl_ReadList();
				Ret[NamedValue.m_Name] = NamedValue.m_Value;
			}
			else
			{
				auto NamedValue = fl_ReadValue();
				Ret[NamedValue.m_Name] = NamedValue.m_Value;
			}
		}
		return Ret;
	}

	bint CValue::f_IsMatch( NStr::CStr const &_Value ) const
	{
		if (mp_ValidValues.f_IsEmpty())
			return true;
		auto It = mp_ValidValues.f_GetIterator();
		for (;It;++It)
		{
			if (*It == _Value)
				return true;
		}
		return false;
	}

	CCommandArguments fg_ParseCommandLine(CParser const &_Parser)
	{
		NContainer::TCVector<NStr::CStr> CommandLineArgs;
		NSys::fg_Process_GetCommandLineArgs(CommandLineArgs);
		return fg_ParseCommandLine(_Parser, CommandLineArgs);
	}

	CCommandArguments fg_ParseCommandLine(CParser const &_Parser,NContainer::TCVector<NStr::CStr> const &_Argv)
	{
		auto fl_SplitIntoOptions = [&](NContainer::TCVector<NStr::CStr> const &_Argv) -> NContainer::TCVector<NContainer::TCVector<NStr::CStr>>
		{
			auto It = _Argv.f_GetIterator();
			NContainer::TCVector<NContainer::TCVector<NStr::CStr>> Ret;
			if (It)
				++It; // skip application name
			while (It)
			{
				if (!_Parser.f_HasOption(*It))
					throw CInvalidArguments(*It);
				NContainer::TCVector<NStr::CStr> Option;
				Option.f_Insert(*It);
				++It;
				while (It && It->f_GetLen() > 0 && (*It)[0] != '-')
				{
					Option.f_Insert(*It);
					++It;
				}
				Ret.f_Insert(fg_Move(Option));
			}
			return Ret;
		};
		CCommandArguments::COptionMap Ret;
		auto const &Options = fl_SplitIntoOptions(_Argv);
		for (NContainer::TCVector<NContainer::TCVector<NStr::CStr>>::CIteratorConst OptionIt = Options.f_GetIterator();OptionIt;++OptionIt)
		{
			NContainer::TCVector<NStr::CStr>::CIteratorConst It = OptionIt->f_GetIterator();
			{
				if (_Parser.f_HasOption(*It))
				{
					COption const &Option = _Parser.f_GetOption(*It);
					++It;
					Ret[Option.f_Name()] = Option.f_ReadValues(It);
					if (!!It)
						throw CInvalidArguments(*It);
				}
				else
					throw CInvalidArguments(*It);
			}
		}
		return fg_Move(Ret);
	}

	NStr::CStr CParser::f_HelpMessage() const
	{
		NStr::CStr  Ret = "Valid options:" DMibNewLine;
		for (auto It = mp_Options.f_GetIterator();It;++It)
		{
			if (It->f_HasShortName())
			{
				Ret += "   -" + It->f_ShortName();
				Ret += " [--" + It->f_Name() + "]";
			}
			else
			{
				Ret += "   --" + It->f_Name();
			}
			Ret += " : " + It->f_Desc();
			Ret += DMibNewLine;
		}
		return Ret;
	}

	bint CParser::f_HasOption( NStr::CStr const &_Name ) const
	{
		if (_Name.f_GetLen()<2)
			return false;
		if (_Name[0]=='-')
		{
			if (_Name[1]=='-')
			{
				return mp_LongNames.f_FindEqual(_Name.f_Extract(2)) != nullptr;
			}
			else
			{
				return mp_ShortNames.f_FindEqual(_Name.f_Extract(1)) != nullptr;
			}
		}
		return false;
	}

	COption const & CParser::f_GetOption( NStr::CStr const &_Name ) const
	{
		DMibRequire(f_HasOption(_Name));
		if (_Name[1]=='-')
		{
			return **mp_LongNames.f_FindEqual(_Name.f_Extract(2));
		}
		return **mp_ShortNames.f_FindEqual(_Name.f_Extract(1));
	}


	CParser::CParser()
	{
		COption Help("help","h","Show this help");
		f_Add(Help);
	}
	void CParser::f_Add(COption const &_Option)
	{
		auto const &Opt = mp_Options.f_Insert(_Option);
		mp_LongNames[_Option.f_Name()] = &Opt;
		mp_ShortNames[_Option.f_ShortName()] = &Opt;
	}

	COption::COption()
	{
	} // make NContainer::TCMap happy
	COption::COption(NStr::CStr const &_Name, NStr::CStr const &_Short, NStr::CStr const &_Description)
		: mp_Name(_Name)
		, mp_ShortName(_Short)
		, mp_Description(_Description)
	{
	}
	void COption::f_Add(CValue const &_Value)
	{
		mp_Values.f_Insert(CArgument(_Value, false));
	}
	void COption::f_AddList(CValue const &_Value)
	{
		mp_Values.f_Insert(CArgument(_Value, true));
	}
	NStr::CStr const &COption::f_Name() const
	{
		return mp_Name;
	}
	NStr::CStr const &COption::f_ShortName() const
	{
		return mp_ShortName;
	}
	bint COption::f_HasShortName() const
	{
		return !mp_ShortName.f_IsEmpty();
	}
	NStr::CStr const &COption::f_Desc() const
	{
		return mp_Description;
	}
	CCommandArguments::CCommandArguments()
		: mp_Values()
	{
	}
	CCommandArguments::CCommandArguments(COptionMap&& _Values)
		: mp_Values(fg_Move(_Values))
	{
	}
	bint CCommandArguments::f_IsSet(NStr::CStr const &_Name) const
	{
		return mp_Values.f_Exists(_Name);
	}
	NContainer::TCMap<NStr::CStr, CCommandArguments::CValue> const &CCommandArguments::operator [] (NStr::CStr const &_Name) const
	{
		DMibRequire(f_IsSet(_Name));
		return mp_Values[_Name];
	}
	bint CCommandArguments::f_HasValue(NStr::CStr const &_Option, NStr::CStr const &_Value) const
	{
		if (!f_IsSet(_Option))
			return false;
		if (!mp_Values[_Option].f_Exists(_Value))
			return false;
		auto Ret = mp_Values[_Option][_Value];
		return Ret.f_IsOfType<NStr::CStr>();
	}
	NStr::CStr CCommandArguments::f_GetValue(NStr::CStr const &_Option, NStr::CStr const &_Value) const
	{
		DMibRequire(f_HasValue(_Option,_Value));
		auto Ret = mp_Values[_Option][_Value];
		return Ret.f_GetAsType<NStr::CStr>();
	}
	bint CCommandArguments::f_HasList(NStr::CStr const &_Option, NStr::CStr const &_Value) const
	{
		if (!f_IsSet(_Option))
			return false;
		if (!mp_Values[_Option].f_Exists(_Value))
			return false;
		auto Ret = mp_Values[_Option][_Value];
		return Ret.f_IsOfType<NContainer::TCVector<NStr::CStr>>();
	}
	NContainer::TCVector<NStr::CStr> CCommandArguments::f_GetList(NStr::CStr const &_Option, NStr::CStr const &_Value) const
	{
		DMibRequire(f_HasList(_Option,_Value));
		auto Ret = mp_Values[_Option][_Value];
		return Ret.f_GetAsType<NContainer::TCVector<NStr::CStr>>();
	}
	bint CCommandArguments::f_HasListItem(NStr::CStr const &_Option, NStr::CStr const &_List, NStr::CStr const &_Item) const
	{
		DMibRequire(f_HasList(_Option, _List));
		auto List = f_GetList(_Option, _List);
		auto It = List.f_GetIterator();
		for (;It;++It)
		{
			if (*It == _Item)
				return true;
		}
		return false;
	}
}
