#pragma once

#include <Common.h>

enum ProcessMemoryRegionFlags
{
	pmrf_None = 0,
	pmrf_Read = (1 << 0),
	pmrf_Write = (1 << 1),
	pmrf_Execute = (1 << 2),
	pmrf_Protect = (1 << 3),

	pmrf_Image = (1 << 4),
	pmrf_Mapped = (1 << 5),
	pmrf_Private = (1 << 6),
};

struct ProcessMemoryRegion
{
	uintptr_t m_start = 0;
	uintptr_t m_end = 0;

	uintptr_t m_moduleBase = 0;
	uintptr_t m_entryPoint = 0;

	int m_flags = pmrf_None;

	s2::string m_path;
	s2::string m_section;

	size_t Size() const;
	bool Contains(uintptr_t p) const;

	bool IsRead() const;
	bool IsWrite() const;
	bool IsExecute() const;
	bool IsProtect() const;

	bool operator ==(const ProcessMemoryRegion& other) const;
	bool operator !=(const ProcessMemoryRegion& other) const;
};
