#pragma once

#include <Common.h>
#include <Tab.h>
#include <Tabs/TaskWaitTab.h>

class StringsTab : public TaskWaitTab
{
public:
	struct Result
	{
		uintptr_t m_code;
		uintptr_t m_string;
	};
	s2::list<Result> m_results;

public:
	StringsTab(Inspector* inspector, const s2::string& name);
	virtual ~StringsTab();

	virtual s2::string GetLabel() override;

	virtual void Render() override;
};
