#pragma once

#include <Common.h>
#include <System/ProcessMemoryRegion.h>

class ProcessHandle
{
public:
	virtual ~ProcessHandle();

	virtual s2::list<ProcessMemoryRegion> GetMemoryRegions() = 0;

	virtual size_t ReadMemory(uintptr_t p, void* buffer, size_t size) = 0;

	bool GetMemoryRegion(uintptr_t p, ProcessMemoryRegion& region);
};
