#pragma once

#include <Common.h>

struct UserInfo
{
	int uid = -1;
	s2::string username;

	bool operator==(const UserInfo& other) { return other.uid == uid; }
};

struct ProcessInfo
{
	s2::string exe;
	s2::string filename;
	int pid = -1;
	UserInfo user;

	bool operator==(const ProcessInfo& other) { return other.pid == pid; }
};
