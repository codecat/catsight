#include <Common.h>
#include <System/ProcessHandle.h>

ProcessHandle::~ProcessHandle()
{
}

s2::string ProcessHandle::ReadCString(uintptr_t p)
{
	const size_t perTime = 255;

	size_t read = 0;
	size_t len = 0;
	static s2::string ret;

	while (true) {
		len += perTime;
		ret.ensure_memory(len);

		ReadMemory(p + read, (void*)ret.c_str() + read, perTime);

		const char* pstr = ret.c_str();
		for (size_t i = read; i < len; i++) {
			if (pstr[i] == '\0') {
				return s2::string(pstr, i);
			}
		}

		read += len;
	}

	return ret;
}

bool ProcessHandle::GetMemoryRegion(uintptr_t p, ProcessMemoryRegion& region)
{
	auto regions = GetMemoryRegions();
	for (auto& r : regions) {
		if (p >= r.m_start && p < r.m_end) {
			region = r;
			return true;
		}
	}
	return false;
}
