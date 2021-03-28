#pragma once

#include <Common.h>
#include <System/UserInfo.h>

struct ProcessInfo
{
	s2::string exe;
	s2::string filename;
	int pid = -1;
	UserInfo user;
	uint64_t startTime = 0;

	bool operator==(const ProcessInfo& other) { return other.pid == pid; }
};
