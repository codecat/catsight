#include <Common.h>
#include <System/ProcessHandle.h>

ProcessHandle::~ProcessHandle()
{
}

void ProcessHandle::ReadCString(uintptr_t p, s2::string& str, int limit)
{
	char buffer[255];

	str = "";

	size_t offset = 0;
	while (true) {
		size_t bytesRead = ReadMemory(p + offset, buffer, sizeof(buffer));

		for (size_t i = 0; i < bytesRead; i++) {
			if (buffer[i] == '\0') {
				str.append(buffer, i);
				return;
			}
		}
		str.append(buffer, sizeof(buffer));

		if ((int)str.len() > limit) {
			str.append("...");
			return;
		}

		offset += bytesRead;
	}
}
