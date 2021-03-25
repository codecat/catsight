#include <Common.h>
#include <System/ProcessHandle.h>

ProcessHandle::~ProcessHandle()
{
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
