#include <Common.h>
#include <System.h>

#include <System/Windows/WindowsProcessHandle.h>

#include <Windows.h>
#include <TlHelp32.h>
#include <shellapi.h>

static UserInfo GetUserFromProcessHandle(HANDLE hProcInfo)
{
	UserInfo ret;

	HANDLE hToken = nullptr;
	if (OpenProcessToken(hProcInfo, TOKEN_QUERY, &hToken)) {
		// First call to GetTokenInformation is to get the required size
		DWORD dwSize = 0;
		GetTokenInformation(hToken, TokenUser, nullptr, 0, &dwSize);

		// We expect this error from the first call
		assert(GetLastError() == ERROR_INSUFFICIENT_BUFFER);

		// Allocate enough space and cast it to the info structure we need
		TOKEN_USER* u = (TOKEN_USER*)alloca(dwSize);

		// Second call should now be okay
		if (GetTokenInformation(hToken, TokenUser, u, dwSize, &dwSize)) {
			char username[256]; DWORD usernameSize = sizeof(username);
			char domain[256]; DWORD domainSize = sizeof(domain);
			SID_NAME_USE sidUse;

			if (LookupAccountSid(nullptr, u->User.Sid, username, &usernameSize, domain, &domainSize, &sidUse)) {
				ret.username = username;
			}
		}

		CloseHandle(hToken);
	}

	return ret;
}

bool System::IsCurrentUserRoot()
{
	bool ret = false;

	HANDLE hToken = nullptr;
	if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken)) {
		TOKEN_ELEVATION e;
		DWORD dwSize = sizeof(TOKEN_ELEVATION);
		if (GetTokenInformation(hToken, TokenElevation, &e, sizeof(e), &dwSize)) {
			ret = (e.TokenIsElevated != 0);
		}
		CloseHandle(hToken);
	}

	return ret;
}

UserInfo System::GetCurrentUser()
{
	UserInfo ret;

	char username[256];
	DWORD usernameSize = sizeof(username);
	GetUserName(username, &usernameSize);
	ret.username = username;

	return ret;
}

UserInfo System::GetProcessUser(int pid)
{
	UserInfo ret;

	HANDLE hProcInfo = OpenProcess(PROCESS_QUERY_INFORMATION, false, pid);
	if (hProcInfo != nullptr) {
		ret = GetUserFromProcessHandle(hProcInfo);
		CloseHandle(hProcInfo);
	}

	return ret;
}

s2::list<ProcessInfo> System::GetProcesses()
{
	s2::list<ProcessInfo> ret;

	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS | TH32CS_SNAPMODULE, 0);

	PROCESSENTRY32 proc;
	if (Process32First(hSnapshot, &proc)) {
		do {
			HANDLE hProcInfo = OpenProcess(PROCESS_QUERY_INFORMATION, false, proc.th32ProcessID);

			// Probably access denied, so we won't even list it
			if (hProcInfo == nullptr) {
				continue;
			}

			char fullPath[MAX_PATH];
			DWORD fullPathSize = sizeof(fullPath);
			if (!QueryFullProcessImageName(hProcInfo, 0, fullPath, &fullPathSize)) {
				continue;
			}

			for (char* p = fullPath; *p != '\0'; p++) {
				if (*p == '\\') {
					*p = '/';
				}
			}

			auto& newProc = ret.add();
			newProc.pathFull = fullPath;
			newProc.pathExe = proc.szExeFile;
			newProc.pathDir = s2::string(fullPath, newProc.pathFull.len() - newProc.pathExe.len() - 1);

			newProc.pid = proc.th32ProcessID;
			newProc.user = GetUserFromProcessHandle(hProcInfo);

			FILETIME tmCreation, tmExit, tmKernel, tmUser;
			if (GetProcessTimes(hProcInfo, &tmCreation, &tmExit, &tmKernel, &tmUser)) {
				ULARGE_INTEGER large;
				large.LowPart = tmCreation.dwLowDateTime;
				large.HighPart = tmCreation.dwHighDateTime;
				newProc.startTime = large.QuadPart;
			}

			CloseHandle(hProcInfo);
		} while (Process32Next(hSnapshot, &proc));
	}

	CloseHandle(hSnapshot);

	return ret;
}

ProcessHandle* System::OpenProcessHandle(const ProcessInfo& info)
{
	return new WindowsProcessHandle(info);
}

void System::Sleep(int ms)
{
	::Sleep(ms);
}

void System::OpenURL(const char* url)
{
	ShellExecuteA(nullptr, nullptr, url, nullptr, nullptr, SW_SHOW);
}
