#pragma once

#include <Common.h>
#include <Tab.h>
#include <System/ProcessMemoryRegion.h>

class MapsTab : public Tab
{
private:
	s2::list<ProcessMemoryRegion> m_maps;

public:
	MapsTab(Inspector* inspector, const s2::string& name);
	virtual ~MapsTab();

	virtual bool CanClose() override;

	virtual void Render() override;
};
