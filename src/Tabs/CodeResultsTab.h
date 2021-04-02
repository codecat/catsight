#pragma once

#include <Common.h>
#include <Tabs/ResultsTab.h>

#include <Zydis/Zydis.h>

class CodeResultsTab : public ResultsTab
{
private:
	ZydisDecoder m_decoder;
	ZydisFormatter m_formatter;

public:
	CodeResultsTab(Inspector* inspector, const s2::string& id);
	virtual ~CodeResultsTab();

	virtual const char* GetTitlePrefix() override;

protected:
	virtual void RenderResult(const Result& result) override;
};
