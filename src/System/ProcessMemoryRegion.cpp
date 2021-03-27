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

bool ProcessMemoryRegion::IsRead()
{
	return (m_flags & pmrf_Read) != 0;
}

bool ProcessMemoryRegion::IsWrite()
{
	return (m_flags & pmrf_Write) != 0;
}

bool ProcessMemoryRegion::IsExecute()
{
	return (m_flags & pmrf_Execute) != 0;
}

bool ProcessMemoryRegion::IsProtect()
{
	return (m_flags & pmrf_Protect) != 0;
}
