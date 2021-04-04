#include <Common.h>
#include <System/ProcessMemoryRegion.h>

size_t ProcessMemoryRegion::Size() const
{
	return m_end - m_start;
}

bool ProcessMemoryRegion::Contains(uintptr_t p) const
{
	return p >= m_start && p < m_end;
}

bool ProcessMemoryRegion::IsRead() const
{
	return (m_flags & pmrf_Read) != 0;
}

bool ProcessMemoryRegion::IsWrite() const
{
	return (m_flags & pmrf_Write) != 0;
}

bool ProcessMemoryRegion::IsExecute() const
{
	return (m_flags & pmrf_Execute) != 0;
}

bool ProcessMemoryRegion::IsProtect() const
{
	return (m_flags & pmrf_Protect) != 0;
}

bool ProcessMemoryRegion::operator ==(const ProcessMemoryRegion& other) const
{
	return m_start == other.m_start && m_end == other.m_end;
}

bool ProcessMemoryRegion::operator !=(const ProcessMemoryRegion& other) const
{
	return !(*this == other);
}
