#pragma once

#include <Common.h>
#include <Tab.h>
#include <System/ProcessMemoryRegion.h>

class MapsTab : public Tab
{
private:
	s2::string m_search = "";

	uintptr_t m_showRegionPointer = 0;
	bool m_showRegionPointerScroll = false;

public:
	MapsTab(Inspector* inspector, const s2::string& name);
	virtual ~MapsTab();

	virtual bool CanClose() override;

	virtual void Render() override;

public:
	void ShowRegionPointer(uintptr_t p);
};
