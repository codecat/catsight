#include <Common.h>
#include <System.h>

#if defined(PLATFORM_WINDOWS)

bool System::IsUserRoot(const UserInfo& info)
{
	//TODO
	return false;
}

UserInfo System::GetCurrentUser()
{
	UserInfo ret;

	//TODO

	return ret;
}

UserInfo System::GetEffectiveUser()
{
	UserInfo ret;

	//TODO

	return ret;
}

UserInfo System::GetProcessUser(int pid)
{
	UserInfo ret;

	//TODO

	return ret;
}

s2::list<ProcessInfo> System::GetProcesses()
{
	s2::list<ProcessInfo> ret;

	//TODO

	return ret;
}

ProcessHandle* System::OpenProcessHandle(const ProcessInfo& info)
{
	//TODO
	return nullptr;
}

#endif
