#include <Common.h>
#include <Tabs/StringsTab.h>
#include <Inspector.h>
#include <Resources.h>
#include <Helpers/CodeButton.h>
#include <Helpers/ImGuiString.h>

#include <hello_imgui.h>

StringsTab::StringsTab(Inspector* inspector, const s2::string& name)
	: TaskWaitTab(inspector, name)
{
	m_results.ensure_memory(10000);
}

StringsTab::~StringsTab()
{
}

s2::string StringsTab::GetLabel()
{
	return s2::strprintf(ICON_FA_TEXT_WIDTH " %s (%d)###Strings", TaskWaitTab::GetLabel().c_str(), (int)m_results.len());
}

void StringsTab::Render(float dt)
{
	TaskWaitTab::Render(dt);

	RenderSearch();
	bool hasFilter = (m_search.len() > 0);

	ImGui::BeginChild("Items");
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 0));

	int numItems = (int)m_results.len();
	if (hasFilter) {
		numItems = (int)m_filterIndices.len();
	}

	ImGuiListClipper clipper;
	clipper.Begin(numItems);
	while (clipper.Step()) {
		for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++) {
			auto& result = hasFilter ? m_results[m_filterIndices[i]] : m_results[i];

			ImGui::PushID(i);

			s2::string str;
			m_inspector->m_processHandle->ReadCString(result.m_string, str);
			str = str.replace("\r", "\\r").replace("\n", "\\n").replace("\t", "\\t");

			Helpers::CodeButton(m_inspector, result.m_code);

			ImGui::SameLine();

			ImGui::PushFont(Resources::FontMono);
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(.5f, .5f, .5f, 1));
			ImGui::Text(POINTER_FORMAT, result.m_code);
			ImGui::PopStyleColor();
			ImGui::PopFont();

			ImGui::SameLine();

			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, .5f, 1));
			ImGui::Text("\"%s\"", str.c_str());
			ImGui::PopStyleColor();

			ImGui::PopID();
		}
	}

	ImGui::PopStyleVar();
	ImGui::EndChild();
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
			m_inspector->m_processHandle->ReadCString(m_results[m_filterIndices[i]].m_string, str);
			if (!str.contains_nocase(m_search)) {
				m_filterIndices.remove(i);
			}
		}
		return;
	}

	m_filterIndices.clear();
	for (int i = 0; i < (int)m_results.len(); i++) {
		m_inspector->m_processHandle->ReadCString(m_results[i].m_string, str);
		if (str.contains_nocase(m_search)) {
			m_filterIndices.add(i);
		}
	}
}
