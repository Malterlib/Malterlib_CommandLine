// Copyright © 2015 Hansoft AB 
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#include "Malterlib_CommandLine.h"
#include "Malterlib_CommandLine_Implementation.h"

#include <Mib/Process/StdIn>

namespace NMib::NCommandLine
{
	template struct TCCommandLineSpecification<CCommandLineSpecificationNoCustomization>;

	DMibImpErrorClassImplement(CCommandLineConvertException);

	COneOfType fg_AnyType()
	{
		return COneOfType
			(
				EEJSONType_String
				, EEJSONType_Integer
				, EEJSONType_Float
				, EEJSONType_Boolean
				, EEJSONType_Object
				, EEJSONType_Array
				, EEJSONType_Date
				, EEJSONType_Binary
				, EEJSONType_UserType
				, COneOf(EEJSONType_Null)
			)
		;
	}

	EAnsiEncodingFlag CCommandLineDefaults::fs_ColorAnsiFlagsDefault()
	{
		EAnsiEncodingFlag Flags = EAnsiEncodingFlag_None;
		if (fs_ColorEnabledDefault())
			Flags |= EAnsiEncodingFlag_Color;
		if (fs_Color24BitEnabledDefault())
			Flags |= EAnsiEncodingFlag_Color24Bit;
		if (fs_ColorLightBackgroundDefault())
			Flags |= EAnsiEncodingFlag_ColorLightBackground;
		if (fs_BoxDrawingDefault())
			Flags |= EAnsiEncodingFlag_BoxDrawing;
		return Flags;
	}

	bool CCommandLineDefaults::fs_ColorEnabledDefault()
	{
		static bool bValue = []
			{
				if (auto Value = fg_GetSys()->f_GetEnvironmentVariable("MalterlibColor", ""))
					return Value == "true";

				if (NSys::fg_System_BeingDebugged())
					return false;

				auto ColorTerm = fg_GetSys()->f_GetEnvironmentVariable("COLORTERM", "");
				if (ColorTerm == "truecolor" || ColorTerm == "24bit")
					return true;

#ifdef DPlatformFamily_Windows
				if (CSystem::ms_PlatformVersion >= 10'0'015063)
					return true;
				else
					return false;
#endif
				return true;
			}
			()
		;
		return bValue;
	}

	bool CCommandLineDefaults::fs_Color24BitEnabledDefault()
	{
		static bool bValue = []
			{
				if (auto Value = fg_GetSys()->f_GetEnvironmentVariable("MalterlibColor24Bit", ""))
					return Value == "true";

				auto ColorTerm = fg_GetSys()->f_GetEnvironmentVariable("COLORTERM", "");
				if (ColorTerm == "truecolor" || ColorTerm == "24bit")
					return true;

#ifdef DPlatformFamily_Windows
				if (CSystem::ms_PlatformVersion >= 10'0'015063)
					return true;
#endif
				return false;
			}
			()
		;
		return bValue;
	}

	bool CCommandLineDefaults::fs_BoxDrawingDefault()
	{
		static bool bValue = []
			{
				if (auto Value = fg_GetSys()->f_GetEnvironmentVariable("MalterlibBoxDrawing", ""))
					return Value == "true";

				return true;
			}
			()
		;
		return bValue;
	}

	bool CCommandLineDefaults::fs_ColorLightBackgroundDefault()
	{
		using namespace NProcess;
		
		static bool bValue = []
			{
				if (auto Value = fg_GetSys()->f_GetEnvironmentVariable("MalterlibColorLight", ""); Value == "true")
					return true;
				else if (Value != "auto")
					return false;

				struct CState
				{
					NThread::CEvent m_Event;
					CStr m_Buffer;
					bool m_bLight = false;
				};

				TCSharedPointer<CState> pState = fg_Construct();

				auto Params = CStdInReaderParams::fs_Create
					(
						[pState](EStdInReaderOutputType _Type, CStrSecure const &_Input)
						{
							if (_Type != EStdInReaderOutputType_StdIn)
								return;
							CByteVector Data((uint8 const *)_Input.f_GetStr(), _Input.f_GetLen());

							pState->m_Buffer += _Input;
							if (auto iFound = pState->m_Buffer.f_Find("\x1B]11;rgb:"); iFound >= 0)
							{
								CStr Buffer = pState->m_Buffer.f_Extract(iFound);

								uint8 Red;
								uint8 Green;
								uint8 Blue;
								aint nParsed;
								(CStr::CParse("\x1B]11;rgb:{nfh}/{nfh}/{nfh}\x07") >> Red >> Green >> Blue).f_Parse(Buffer, nParsed);
								if (nParsed == 3)
								{
									pState->m_bLight = (fp32(Red) * 0.3f + fp32(Green) * 0.59f + fp64(Blue) * 0.11f) > 128.0f;
									pState->m_Event.f_SetSignaled();
								}
							}
						}
					)
				;

				CStdInReader StdIn(fg_Move(Params));

				DMibConOut2("\x1B]11;?\x1B\\");

				if (pState->m_Event.f_WaitTimeout(1.0))
					return false;

				return pState->m_bLight;
			}
			()
		;
		return bValue;
	}

	NCommandLine::EAnsiEncodingFlag CCommandLineDefaults::fs_ParseAnsiEncodingParams(NEncoding::CEJSONSorted const &_Params)
	{
		NCommandLine::EAnsiEncodingFlag AnsiFlags = EAnsiEncodingFlag_None;
		if (_Params.f_GetMemberValue("Color", CCommandLineDefaults::fs_ColorEnabledDefault()).f_Boolean())
			AnsiFlags |= EAnsiEncodingFlag_Color;
		if (_Params.f_GetMemberValue("Color24Bit", CCommandLineDefaults::fs_Color24BitEnabledDefault()).f_Boolean())
			AnsiFlags |= EAnsiEncodingFlag_Color24Bit;
		if (_Params.f_GetMemberValue("ColorLight", CCommandLineDefaults::fs_ColorLightBackgroundDefault()).f_Boolean())
			AnsiFlags |= EAnsiEncodingFlag_ColorLightBackground;
		if (_Params.f_GetMemberValue("BoxDrawing", CCommandLineDefaults::fs_BoxDrawingDefault()).f_Boolean())
			AnsiFlags |= EAnsiEncodingFlag_BoxDrawing;

		return AnsiFlags;
	}
}
