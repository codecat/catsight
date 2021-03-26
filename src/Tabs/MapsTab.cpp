#include <Common.h>
#include <Tabs/MapsTab.h>

#include <Inspector.h>
#include <Resources.h>
#include <Helpers/MemoryButton.h>

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
	if (ImGui::IsWindowAppearing()) {
		m_maps = m_inspector->m_processHandle->GetMemoryRegions();
	}

	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 0));

	for (auto& map : m_maps) {
		ImGui::PushID(map.m_start);
		ImGui::PushFont(Resources::FontMono);

		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(.5f, .5f, .5f, 1));
		ImGui::Text(POINTER_FORMAT " - " POINTER_FORMAT, map.m_start, map.m_end);
		ImGui::PopStyleColor();

		ImGui::SameLine(270);
		if (map.m_flags & pmrf_Read) {
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(.5f, 1, .5f, 1));
			ImGui::Text("R");
			ImGui::PopStyleColor();
		}

		ImGui::SameLine(290);
		if (map.m_flags & pmrf_Write) {
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, .5f, 1));
			ImGui::Text("W");
			ImGui::PopStyleColor();
		}

		ImGui::SameLine(310);
		if (map.m_flags & pmrf_Execute) {
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, .5f, .5f, 1));
			ImGui::Text("X");
			ImGui::PopStyleColor();
		}

		ImGui::SameLine(330);
		if (map.m_flags & pmrf_Protect) {
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(.5f, .5f, 1, 1));
			ImGui::Text("P");
			ImGui::PopStyleColor();
		}

		ImGui::PopFont();

		ImGui::SameLine(350);

		Helpers::MemoryButton(m_inspector, map.m_start, "Memory");

		ImGui::SameLine();
		ImGui::TextUnformatted(map.m_path);

		ImGui::PopID();
	}

	ImGui::PopStyleVar();
}
