#pragma once

#include <Common.h>
#include <System/ProcessInfo.h>
#include <System/ProcessHandle.h>

namespace System
{
	bool IsCurrentUserRoot();
	UserInfo GetCurrentUser();

	UserInfo GetProcessUser(int pid);
	s2::list<ProcessInfo> GetProcesses();

	ProcessHandle* OpenProcessHandle(const ProcessInfo& info);
}
