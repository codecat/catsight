#include <System/Windows/WindowsError.h>

#include <Windows.h>

void System::Windows::CheckLastError()
{
	DWORD dwError = GetLastError();
	if (dwError != NO_ERROR) {
		LPTSTR errorText = nullptr;

		FormatMessage(
			FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS,
			nullptr,
			dwError,
			MAKELANGID(LANG_ENGLISH, SUBLANG_DEFAULT),
			(LPTSTR)&errorText,
			0,
			nullptr
		);

		printf("Windows error %d: %s", dwError, errorText);

		LocalFree(errorText);
	}
}
