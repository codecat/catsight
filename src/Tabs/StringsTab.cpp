#include <Common.h>
#include <Tabs/StringsTab.h>
#include <Inspector.h>
#include <Resources.h>
#include <Helpers/CodeButton.h>
#include <Helpers/ImGuiString.h>

#include <hello_imgui.h>

StringsTab::StringsTab(Inspector* inspector, const s2::string& id)
	: CodeResultsTab(inspector, id)
{
	m_results.ensure_memory(10000);
}

StringsTab::~StringsTab()
{
}

const char* StringsTab::GetTitlePrefix()
{
	return ICON_FA_TEXT_WIDTH;
}

void StringsTab::Render(float dt)
{
	RenderSearch();

	CodeResultsTab::Render(dt);
}

void StringsTab::RenderResult(const Result& result)
{
	CodeResultsTab::RenderResult(result);

	ImGui::SameLine(500.0f);

	s2::string str;
	m_inspector->m_processHandle->ReadCString(result.m_value, str);
	str = str.replace("\r", "\\r").replace("\n", "\\n").replace("\t", "\\t");

	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, .5f, 1));
	ImGui::Text("\"%s\"", str.c_str());
	ImGui::PopStyleColor();
}

size_t StringsTab::GetNumResults()
{
	if (m_search.len() > 0) {
		return m_filterIndices.len();
	}
	return m_results.len();
}

const StringsTab::Result& StringsTab::GetResult(size_t i)
{
	if (m_search.len() > 0) {
		return m_results[m_filterIndices[i]];
	}
	return m_results[i];
}

void StringsTab::RenderSearch()
{
	size_t prevSearchLen = m_search.len();
	bool changed = Helpers::InputText("Search", &m_search, ImGuiInputTextFlags_AutoSelectAll);

	if (!changed) {
		return;
	}

	if (m_search == "") {
		m_filterIndices.clear();
		return;
	}

	s2::string str;

	if (m_filterIndices.len() > 0 && m_search.len() > prevSearchLen) {
		for (int i = (int)m_filterIndices.len() - 1; i >= 0; i--) {
			m_inspector->m_processHandle->ReadCString(m_results[m_filterIndices[i]].m_value, str);
			if (!str.contains_nocase(m_search)) {
				m_filterIndices.remove(i);
			}
		}
		return;
	}

	m_filterIndices.clear();
	for (int i = 0; i < (int)m_results.len(); i++) {
		m_inspector->m_processHandle->ReadCString(m_results[i].m_value, str);
		if (str.contains_nocase(m_search)) {
			m_filterIndices.add(i);
		}
	}
}
