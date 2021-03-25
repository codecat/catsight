#pragma once

#include <Common.h>
#include <System/ProcessInfo.h>
#include <System/ProcessHandle.h>

namespace System
{
	bool IsUserRoot(const UserInfo& info);

	UserInfo GetCurrentUser();
	UserInfo GetEffectiveUser();

	UserInfo GetProcessUser(int pid);
	s2::list<ProcessInfo> GetProcesses();

	ProcessHandle* OpenProcessHandle(const ProcessInfo& info);
}
