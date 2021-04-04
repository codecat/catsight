#pragma once

#include <Common.h>
#include <Tab.h>
#include <System/ProcessMemoryRegion.h>

class MapsTab : public Tab
{
private:
	s2::string m_search;
	s2::list<size_t> m_filterIndices;

	uintptr_t m_showRegionPointer = 0;
	bool m_showRegionPointerScroll = false;
	float m_showRegionPointerTime = 0;

public:
	MapsTab(Inspector* inspector, const s2::string& id);
	virtual ~MapsTab();

	virtual const char* GetTitlePrefix() override;

	virtual bool CanClose() override;

	virtual void Render(float dt) override;

public:
	void ShowRegionPointer(uintptr_t p);

private:
	void RenderSearch();
	void DoSearch();
};
