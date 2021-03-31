#include <Common.h>
#include <Tabs/MapsTab.h>

#include <Inspector.h>
#include <Resources.h>
#include <Helpers/DataButton.h>
#include <Helpers/CodeButton.h>
#include <Helpers/ImGuiString.h>

#include <hello_imgui.h>

MapsTab::MapsTab(Inspector* inspector, const s2::string& name)
	: Tab(inspector, name)
{
}

MapsTab::~MapsTab()
{
}

bool MapsTab::CanClose()
{
	return false;
}

void MapsTab::Render()
{
	//TODO: Maybe pick a better way to refresh regions for an inspector rather than opening the maps tab
	if (ImGui::IsWindowAppearing()) {
		m_inspector->m_processRegions = m_inspector->m_processHandle->GetMemoryRegions();
	}

	Helpers::InputText("Search", &m_search, ImGuiInputTextFlags_AutoSelectAll);

	ImGui::BeginChild("Maps");

	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 0));

	if (ImGui::BeginTable("Maps", 3)) {
		ImGui::TableSetupColumn("Range", ImGuiTableColumnFlags_WidthFixed);
		ImGui::TableSetupColumn("Flags", ImGuiTableColumnFlags_WidthFixed);
		ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
		ImGui::TableHeadersRow();

		for (auto& map : m_inspector->m_processRegions) {
			if (m_search != "") {
				if (!map.m_path.contains_nocase(m_search) && !map.m_section.contains_nocase(m_search)) {
					continue;
				}
			}

			ImGui::TableNextRow();

			ImGui::PushID(map.m_start);
			ImGui::PushFont(Resources::FontMono);

			ImGui::TableSetColumnIndex(0);
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(.5f, .5f, .5f, 1));
			ImGui::Text(POINTER_FORMAT " - " POINTER_FORMAT, map.m_start, map.m_end);
			ImGui::PopStyleColor();

			ImGui::TableSetColumnIndex(1);
			if (map.IsRead()) {
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(.5f, 1, .5f, 1));
				ImGui::Text("R");
				ImGui::PopStyleColor();
			}

			ImGui::SameLine(20);
			if (map.IsWrite()) {
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, .5f, 1));
				ImGui::Text("W");
				ImGui::PopStyleColor();
			}

			ImGui::SameLine(40);
			if (map.IsExecute()) {
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, .5f, .5f, 1));
				ImGui::Text("X");
				ImGui::PopStyleColor();
			}

			ImGui::SameLine(60);
			if (map.IsProtect()) {
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(.5f, .5f, 1, 1));
				ImGui::Text("P");
				ImGui::PopStyleColor();
			}

			ImGui::PopFont();

			ImGui::TableSetColumnIndex(2);
			if (map.IsExecute()) {
				uintptr_t codePointer = map.m_start;
				if (map.m_entryPoint != 0) {
					codePointer = map.m_entryPoint;
				}
				Helpers::CodeButton(m_inspector, codePointer);
				ImGui::SameLine();
			}
			Helpers::DataButton(m_inspector, map.m_start);

			if (map.m_section != "") {
				ImGui::SameLine();
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(.5f, .5f, .5f, 1));
				ImGui::TextUnformatted(map.m_section);
				ImGui::PopStyleColor();
			} else if (map.m_path != "") {
				ImGui::SameLine();
				ImGui::TextUnformatted(map.m_path);
			}

			ImGui::PopID();
		}
		ImGui::EndTable();
	}

	ImGui::PopStyleVar();

	ImGui::EndChild();
}
