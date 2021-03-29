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

void StringsTab::Render()
{
	TaskWaitTab::Render();

	ImGui::BeginChild("Items");
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 0));

	ImGuiListClipper clipper;
	clipper.Begin((int)m_results.len());
	while (clipper.Step()) {
		for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++) {
			auto& result = m_results[i];

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
