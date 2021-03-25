#include <Common.h>
#include <System.h>
#include <Folder.h>

#if defined(PLATFORM_LINUX)
#  include <pwd.h>
#  include <unistd.h>
#endif

bool System::IsUserRoot(const UserInfo& info)
{
#if defined(MANIA_WINDOWS)
	//TODO
	return false;
#else
	return (info.uid == 0);
#endif
}

UserInfo System::GetCurrentUser()
{
	UserInfo ret;

#if defined(PLATFORM_WINDOWS)
	//TODO
#elif defined(PLATFORM_LINUX)
	ret.uid = (int)getuid();
	auto pwd = getpwuid((uid_t)ret.uid);
	if (pwd != nullptr) {
		ret.username = pwd->pw_name;
	}
#endif

	return ret;
}

UserInfo System::GetEffectiveUser()
{
	UserInfo ret;

#if defined(PLATFORM_WINDOWS)
	//TODO
#elif defined(PLATFORM_LINUX)
	ret.uid = (int)geteuid();
	auto pwd = getpwuid((uid_t)ret.uid);
	if (pwd != nullptr) {
		ret.username = pwd->pw_name;
	}
#endif

	return ret;
}

UserInfo System::GetProcessUser(int pid)
{
	UserInfo ret;

#if defined(PLATFORM_WINDOWS)
	//TODO
#elif defined(PLATFORM_LINUX)
	FILE* fh = fopen(s2::strprintf("/proc/%d/status", pid), "r");
	if (fh != nullptr) {
		char buffer[512];
		while (!feof(fh)) {
			if (fgets(buffer, sizeof(buffer), fh) == nullptr) {
				break;
			}

			s2::stringsplit parse(buffer, "\t");
			if (parse.len() < 2) {
				continue;
			}

			if (parse[0] == "Uid:" && parse.len() == 5) {
				ret.uid = atoi(parse[2]); // effective UID
				auto pwd = getpwuid((uid_t)ret.uid);
				if (pwd != nullptr) {
					ret.username = pwd->pw_name;
					printf("username %s\n", ret.username.c_str());
				}
				break;
			}
		}
		fclose(fh);
	}
#endif

	return ret;
}

s2::list<ProcessInfo> System::GetProcesses()
{
	s2::list<ProcessInfo> ret;

#if defined(PLATFORM_WINDOWS)
	//TODO
#elif defined(PLATFORM_LINUX)
	char linkBuffer[256];

	FolderIndex fi("/proc/", false);
	for (int i = 0; i < fi.GetDirCount(); i++) {
		s2::string procPath = fi.GetDirPath(i);

		int pid = -1;
		sscanf(procPath.c_str(), "/proc/%d/", &pid);
		if (pid == -1) {
			continue;
		}

		ssize_t s = readlink(procPath + "exe", linkBuffer, sizeof(linkBuffer));
		if (s == -1) {
			continue;
		}
		linkBuffer[s] = '\0';

		for (ssize_t i = 0; i < s; i++) {
			if (linkBuffer[i] == '\\') {
				linkBuffer[i] = '/';
			}
		}

		const char* linkBufferFilename = strrchr(linkBuffer, '/');
		if (linkBufferFilename == nullptr) {
			linkBufferFilename = linkBuffer;
		} else {
			linkBufferFilename++;
		}

		auto& newProcess = ret.push();
		newProcess.exe = linkBuffer;
		newProcess.filename = linkBufferFilename;
		newProcess.pid = pid;
		newProcess.user = GetProcessUser(pid);
	}
#endif

	ret.sort([](const void* pa, const void* pb) {
		auto& a = *(ProcessInfo*)pa;
		auto& b = *(ProcessInfo*)pb;
		if (a.pid < b.pid) {
			return 1;
		} else if (a.pid > b.pid) {
			return -1;
		}
		return 0;
	});

	return ret;
}
