#pragma once

#include <Common.h>
#include <Tabs/TaskWaitTab.h>

class ResultsTab : public TaskWaitTab
{
public:
	struct Result
	{
		uintptr_t m_address = 0;
		uintptr_t m_value = 0;
	};
	s2::list<Result> m_results;

public:
	ResultsTab(Inspector* inspector, const s2::string& name);
	virtual ~ResultsTab();

	virtual void Render(float dt) override;

protected:
	virtual void RenderResult(const Result& result);

	virtual size_t GetNumResults();
	virtual const Result& GetResult(size_t i);

private:
	void RenderSearch();
};
