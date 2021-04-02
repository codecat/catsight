#pragma once

#include <Common.h>
#include <Tabs/ResultsTab.h>
#include <Disassembler.h>

class CodeResultsTab : public ResultsTab
{
private:
	Disassembler m_disasm;

public:
	CodeResultsTab(Inspector* inspector, const s2::string& id);
	virtual ~CodeResultsTab();

	virtual const char* GetTitlePrefix() override;

protected:
	virtual void RenderResult(const Result& result) override;
};
