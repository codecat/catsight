#pragma once

#include <Common.h>

struct UserInfo
{
	int uid = -1;
	s2::string username;

	bool operator==(const UserInfo& other) { return other.uid == uid; }
};
