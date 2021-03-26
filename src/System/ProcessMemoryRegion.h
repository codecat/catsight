#pragma once

#include <Common.h>

enum ProcessMemoryRegionFlags
{
	pmrf_None = 0,
	pmrf_Read = (1 << 0),
	pmrf_Write = (1 << 1),
	pmrf_Execute = (1 << 2),
	pmrf_Protect = (1 << 3),
};

struct ProcessMemoryRegion
{
	uintptr_t m_start = 0;
	uintptr_t m_end = 0;

	int m_flags = pmrf_None;

	s2::string m_path;

	size_t Size();
	bool Contains(uintptr_t p);
};
