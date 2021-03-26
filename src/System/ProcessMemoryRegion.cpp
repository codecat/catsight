#include <Common.h>
#include <System/ProcessMemoryRegion.h>

size_t ProcessMemoryRegion::Size()
{
	return m_end - m_start;
}

bool ProcessMemoryRegion::Contains(uintptr_t p)
{
	return p >= m_start && p < m_end;
}
