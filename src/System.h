#pragma once

#include <Common.h>
#include <ProcessInfo.h>

namespace System
{
	bool IsUserRoot(const UserInfo& info);

	UserInfo GetCurrentUser();
	UserInfo GetEffectiveUser();

	UserInfo GetProcessUser(int pid);
	s2::list<ProcessInfo> GetProcesses();
}
