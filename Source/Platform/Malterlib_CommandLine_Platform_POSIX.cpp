// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

using namespace NMib;

#include "../Malterlib_CommandLine_Platform.h"

#include <Mib/Core/PlatformSpecific/PosixFork>
#ifdef DPlatformFamily_Linux
#	include <Mib/Core/PlatformSpecific/LinuxOptional>
#endif

#include <fcntl.h>
#include <poll.h>
#include <signal.h>
#include <unistd.h>

namespace
{
	constinit static int gs_SignalPipe[2] = {-1, -1};

	// A signal handler may only use async signal safe facilities; writing to the self pipe is,
	// while the framework event and semaphore primitives are not (their signal paths take locks).
	// The watcher thread blocks on the pipe and turns wakeups into callbacks on a normal thread.
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
			while (true)
			{
				pollfd PollFd = {};
				PollFd.fd = gs_SignalPipe[0];
				PollFd.events = POLLIN;

				if (poll(&PollFd, 1, -1) < 0)
				{
					if (errno == EINTR)
						continue;

					break;
				}

				// Drain the pipe in whole so a burst of resizes coalesces into one callback
				bool bQuit = false;
				bool bChanged = false;
				ch8 Bytes[64];

				while (true)
				{
					auto nRead = read(gs_SignalPipe[0], Bytes, sizeof(Bytes));
					if (nRead <= 0)
						break;

					for (auto i = decltype(nRead)(0); i < nRead; ++i)
					{
						if (Bytes[i] == 'q')
							bQuit = true;
						else
							bChanged = true;
					}
				}

				if (bChanged)
					m_fOnScreenChange(NSys::fg_GetConsoleProperties());

				if (bQuit)
					break;
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

	// Nonblocking reads let the watcher drain bursts, and a nonblocking write keeps the signal
	// handler from ever blocking on a full pipe (a pending byte already guarantees a wakeup)
	int Pipes[2];
#ifdef DPlatformFamily_Linux
	if (NLocal::g_f_pipe2)
	{
		if (NLocal::g_f_pipe2(Pipes, O_CLOEXEC | O_NONBLOCK))
		{
			using namespace NMib::NStr;

			DMibError("pipe2 (screen change watcher) failed with errno {}"_f << errno);
		}
	}
	else
#endif
	{
		// We need to make sure that we protect the pipes against being included in other processes
		DMibLock(::NMib::NPlatform::fg_ForkLock());
		if (pipe(Pipes))
		{
			using namespace NMib::NStr;

			DMibError("pipe (screen change watcher) failed with errno {}"_f << errno);
		}

		fcntl(Pipes[0], F_SETFD, fcntl(Pipes[0], F_GETFD) | FD_CLOEXEC);
		fcntl(Pipes[1], F_SETFD, fcntl(Pipes[1], F_GETFD) | FD_CLOEXEC);
		fcntl(Pipes[0], F_SETFL, fcntl(Pipes[0], F_GETFL) | O_NONBLOCK);
		fcntl(Pipes[1], F_SETFL, fcntl(Pipes[1], F_GETFL) | O_NONBLOCK);
	}

#ifdef F_SETNOSIGPIPE
	fcntl(Pipes[0], F_SETNOSIGPIPE, 1);
	fcntl(Pipes[1], F_SETNOSIGPIPE, 1);
#endif

	gs_SignalPipe[0] = Pipes[0];
	gs_SignalPipe[1] = Pipes[1];

	NStorage::TCSharedPointer<CScreenChangeWatcher> pWatcher = fg_Construct(fg_Move(_fOnScreenChange));
	pWatcher->f_Start();

	static auto fSigHandler = [](int const _Signal)
		{
			ch8 Byte = 'r';
			(void)!write(gs_SignalPipe[1], &Byte, 1);
		}
	;

	auto fOldSignal = signal(SIGWINCH, (sig_t)fSigHandler);

	return g_OnScopeExitShared / [pWatcher, fOldSignal]
		{
			signal(SIGWINCH, fOldSignal);

			ch8 Byte = 'q';
			(void)!write(gs_SignalPipe[1], &Byte, 1);

			pWatcher->f_Stop(true);

			close(gs_SignalPipe[0]);
			close(gs_SignalPipe[1]);
			gs_SignalPipe[0] = -1;
			gs_SignalPipe[1] = -1;

			gs_bScreenChangeWatcherInstalled = false;
		}
	;
}
