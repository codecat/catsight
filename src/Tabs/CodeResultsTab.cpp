#include <Common.h>
#include <Tabs/CodeResultsTab.h>
#include <Inspector.h>
#include <Helpers/CodeButton.h>

#include <hello_imgui.h>

CodeResultsTab::CodeResultsTab(Inspector* inspector, const s2::string& id)
	: ResultsTab(inspector, id)
{
}

CodeResultsTab::~CodeResultsTab()
{
}

const char* CodeResultsTab::GetTitlePrefix()
{
	return ICON_FA_SEARCH;
}

void CodeResultsTab::RenderResult(const Result& result)
{
	Helpers::CodeButton(m_inspector, result.m_address);

	ImGui::SameLine();

	ResultsTab::RenderResult(result);

	ImGui::SameLine();

	ZydisDecodedInstruction instr;
	if (m_disasm.Decode(instr, m_inspector->m_processHandle, result.m_address)) {
		ImGui::TextUnformatted(m_disasm.Format(instr, result.m_address));
	}
}
