#pragma once

#include <Common.h>
#include <System/ProcessInfo.h>
#include <System/ProcessHandle.h>
#include <Tasks/Tasks.h>
#include <Tab.h>

class Inspector
{
private:
	s2::string m_title;

public:
	bool m_isOpen = true;

	Tasks m_tasks;

	ProcessInfo m_processInfo;
	ProcessHandle* m_processHandle = nullptr;
	s2::list<ProcessMemoryRegion> m_processRegions;

	s2::list<Tab*> m_tabs;

public:
	Inspector(const ProcessInfo& info);
	~Inspector();

	const ProcessInfo& GetProcessInfo();

	bool GetMemoryRegion(uintptr_t p, ProcessMemoryRegion& region);

	void Render();
	void Update();

private:
	void RenderMenu();
	void RenderTabs();
};
