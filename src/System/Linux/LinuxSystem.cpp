#include <Common.h>
#include <System.h>

#include <Folder.h>
#include <System/Linux/LinuxProcessHandle.h>

#include <pwd.h>
#include <unistd.h>

bool System::IsCurrentUserRoot()
{
	return (GetCurrentUser().uid == 0);
}

UserInfo System::GetCurrentUser()
{
	UserInfo ret;

	ret.uid = (int)getuid();
	auto pwd = getpwuid((uid_t)ret.uid);
	if (pwd != nullptr) {
		ret.username = pwd->pw_name;
	}

	return ret;
}

UserInfo System::GetProcessUser(int pid)
{
	UserInfo ret;

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
				}
				break;
			}
		}
		fclose(fh);
	}

	return ret;
}

s2::list<ProcessInfo> System::GetProcesses()
{
	s2::list<ProcessInfo> ret;

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

		//TODO: Why is this here? This looks wrong! Do we need this on Linux for some reason?
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
		newProcess.pathFull = linkBuffer;
		newProcess.pathExe = linkBufferFilename;
		newProcess.pathDir = s2::string(linkBuffer, newProc.pathFull.len() - newProc.pathExe.len() - 1);
		newProcess.pid = pid;
		newProcess.user = GetProcessUser(pid);
	}

	return ret;
}

ProcessHandle* System::OpenProcessHandle(const ProcessInfo& info)
{
	return new LinuxProcessHandle(info);
}

void System::Sleep(int ms)
{
	usleep(ms * 1000);
}
