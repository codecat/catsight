#pragma once

#include <Common.h>
#include <System/UserInfo.h>

struct ProcessInfo
{
	s2::string pathFull;
	s2::string pathExe;
	s2::string pathDir;

	int pid = -1;
	UserInfo user;
	uint64_t startTime = 0;

	bool operator==(const ProcessInfo& other) const;
};
