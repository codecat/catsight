#pragma once

#include <Common.h>

struct UserInfo
{
	int uid = -1;
	s2::string username;

	bool operator==(const UserInfo& other)
	{
		if (uid != -1 && other.uid == uid) {
			return true;
		}

		if (other.username == username) {
			return true;
		}

		return false;
	}
};
