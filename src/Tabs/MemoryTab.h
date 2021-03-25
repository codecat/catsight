#pragma once

#include <Common.h>
#include <Tab.h>
#include <System/ProcessHandle.h>

class MemoryTab : public Tab
{
public:
	bool m_hasValidRegion;
	ProcessMemoryRegion m_region;

	uintptr_t m_baseOffset = 0;

public:
	MemoryTab(Inspector* inspector, const s2::string& name, uintptr_t p);
	virtual ~MemoryTab();

	virtual void Render() override;
};
