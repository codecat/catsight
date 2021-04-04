#include <Common.h>
#include <System/ProcessInfo.h>

bool ProcessInfo::operator==(const ProcessInfo& other) const
{
	return other.pid == pid;
}
