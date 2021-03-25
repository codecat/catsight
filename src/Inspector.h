#pragma once

#include <Common.h>
#include <ProcessInfo.h>

class Inspector
{
private:
	ProcessInfo m_processInfo;
	s2::string m_title;

public:
	bool m_isOpen = true;

public:
	Inspector(const ProcessInfo& info);

	const ProcessInfo& GetProcessInfo();

	void Render();
};
