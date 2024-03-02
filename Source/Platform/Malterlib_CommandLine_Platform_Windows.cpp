// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

using namespace NMib;

#include "../Malterlib_CommandLine_Platform.h"

namespace
{
	// Windows has no console resize signal and the stdin reader owns the console input queue
	// (it discards non-key events such as WINDOW_BUFFER_SIZE_EVENT), so poll the console size.
	struct CScreenChangeWatcher : public NThread::CThread
	{
		CScreenChangeWatcher(NFunction::TCFunction<void (NSys::CConsoleProperties const &_ConsoleProperties)> &&_fOnScreenChange)
			: m_fOnScreenChange(fg_Move(_fOnScreenChange))
		{
		}
		~CScreenChangeWatcher()
		{
			f_Stop(true);
		}

	private:
		NStr::CStr f_GetThreadName()
		{
			return "Screen change watcher";
		}

		aint f_Main()
		{
			auto LastProperties = NSys::fg_GetConsoleProperties();

			while (f_GetState() != NThread::EThreadState_EventWantQuit)
			{
				m_EventWantQuit.f_WaitTimeout(0.05);

				auto ConsoleProperties = NSys::fg_GetConsoleProperties();
				if
					(
						ConsoleProperties.m_Width == LastProperties.m_Width
						&& ConsoleProperties.m_Height == LastProperties.m_Height
						&& ConsoleProperties.m_GlyphWidth == LastProperties.m_GlyphWidth
						&& ConsoleProperties.m_GlyphHeight == LastProperties.m_GlyphHeight
					)
				{
					continue;
				}

				LastProperties = ConsoleProperties;
				m_fOnScreenChange(ConsoleProperties);
			}

			return 0;
		}

		NFunction::TCFunction<void (NSys::CConsoleProperties const &_ConsoleProperties)> m_fOnScreenChange;
	};

	constinit static bool gs_bScreenChangeWatcherInstalled = false;
}

NMib::COnScopeExitShared NMib::NCommandLine::NPlatform::fg_Process_WaitForScreenChange(NFunction::TCFunction<void (NSys::CConsoleProperties const &_ConsoleProperties)> &&_fOnScreenChange)
{
	if (gs_bScreenChangeWatcherInstalled)
		DMibError("You can only install one wait for screen change handler");

	gs_bScreenChangeWatcherInstalled = true;

	NStorage::TCSharedPointer<CScreenChangeWatcher> pWatcher = fg_Construct(fg_Move(_fOnScreenChange));
	pWatcher->f_Start();

	return g_OnScopeExitShared / [pWatcher]
		{
			pWatcher->f_Stop(true);
			gs_bScreenChangeWatcherInstalled = false;
		}
	;
}
