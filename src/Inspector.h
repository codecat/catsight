#pragma once

#include <Common.h>
#include <ProcessInfo.h>
#include <Tab.h>

class Inspector
{
private:
	ProcessInfo m_processInfo;
	s2::string m_title;

public:
	bool m_isOpen = true;
	s2::list<Tab*> m_tabs;

public:
	Inspector(const ProcessInfo& info);

	const ProcessInfo& GetProcessInfo();

	void RenderMemory(int index);
	void Render();
};
