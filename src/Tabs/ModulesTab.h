#pragma once

#include <Common.h>
#include <Tab.h>
#include <System/ProcessMemoryRegion.h>

class ModulesTab : public Tab
{
private:
	struct Module
	{
		s2::string m_path;
		s2::list<ProcessMemoryRegion> m_regions;
	};
	s2::list<Module> m_modules;

public:
	ModulesTab(Inspector* inspector, const s2::string& id);
	virtual ~ModulesTab();

	virtual const char* GetTitlePrefix() override;

	virtual bool CanClose() override;

	virtual void Render(float dt) override;

private:
	Module* FindModule(const s2::string& path);
};
