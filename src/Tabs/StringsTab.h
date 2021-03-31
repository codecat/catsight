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

	s2::list<int> m_filterIndices;
	s2::string m_search = "";

public:
	StringsTab(Inspector* inspector, const s2::string& name);
	virtual ~StringsTab();

	virtual s2::string GetLabel() override;

	virtual void Render() override;

private:
	void RenderSearch();
};
