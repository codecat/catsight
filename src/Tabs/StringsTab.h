#pragma once

#include <Common.h>
#include <Tabs/CodeResultsTab.h>

class StringsTab : public CodeResultsTab
{
private:
	s2::list<int> m_filterIndices;
	s2::string m_search;

public:
	StringsTab(Inspector* inspector, const s2::string& name);
	virtual ~StringsTab();

	virtual s2::string GetLabel() override;

	virtual void Render(float dt) override;

protected:
	virtual void RenderResult(const Result& result) override;

	virtual size_t GetNumResults() override;
	virtual const Result& GetResult(size_t i) override;

private:
	void RenderSearch();
};
