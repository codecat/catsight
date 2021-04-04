#include <Common.h>
#include <Tabs/MapsTab.h>

#include <Inspector.h>
#include <Resources.h>
#include <Helpers/DataButton.h>
#include <Helpers/CodeButton.h>
#include <Helpers/ImGuiString.h>

#include <hello_imgui.h>

MapsTab::MapsTab(Inspector* inspector, const s2::string& id)
	: Tab(inspector, id)
{
}

MapsTab::~MapsTab()
{
}

const char* MapsTab::GetTitlePrefix()
{
	return ICON_FA_MAP;
}

bool MapsTab::CanClose()
{
	return false;
}

void MapsTab::Render(float dt)
{
	//TODO: Maybe pick a better way to refresh regions for an inspector rather than opening the maps tab
	if (ImGui::IsWindowAppearing()) {
		std::scoped_lock lock(m_inspector->m_processRegionsMutex);
		m_inspector->m_processRegions = m_inspector->m_processHandle->GetMemoryRegions();
		DoSearch();
	}

	RenderSearch();

	ImGui::BeginChild("Maps");

	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 0));

	if (m_showRegionPointerTime > 0) {
		m_showRegionPointerTime -= dt;
		if (m_showRegionPointerTime <= 0) {
			m_showRegionPointer = 0;
		}
	}

	if (ImGui::BeginTable("Maps", 3)) {
		ImGui::TableSetupColumn("Range", ImGuiTableColumnFlags_WidthFixed);
		ImGui::TableSetupColumn("Flags", ImGuiTableColumnFlags_WidthFixed, 70);
		ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
		ImGui::TableHeadersRow();

		int numItems = (int)m_inspector->m_processRegions.len();
		bool isSearching = (m_search != "");
		if (isSearching) {
			numItems = (int)m_filterIndices.len();
		}

		ImGuiListClipper clipper;
		clipper.Begin(numItems);
		while (clipper.Step()) {
			for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++) {
				int index = isSearching ? m_filterIndices[i] : i;
				auto& map = m_inspector->m_processRegions[index];

				ImGui::TableNextRow();

				bool highlight = false;
				if (m_showRegionPointer != 0 && map.Contains(m_showRegionPointer)) {
					if (m_showRegionPointerScroll) {
						ImGui::SetScrollHereY();
						m_showRegionPointerScroll = false;
					}
					highlight = true;
				}

				ImGui::PushID(map.m_start);

				ImGui::PushFont(Resources::FontMono);

				ImGui::TableSetColumnIndex(0);
				if (highlight) {
					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, .5f, .5f, 1));
				} else {
					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(.5f, .5f, .5f, 1));
				}
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
		}

		ImGui::EndTable();
	}

	ImGui::PopStyleVar();

	ImGui::EndChild();
}

void MapsTab::ShowRegionPointer(uintptr_t p)
{
	m_showRegionPointer = p;
	m_showRegionPointerScroll = true;
	m_showRegionPointerTime = 1000.0f;

	m_search = "";
}

void MapsTab::RenderSearch()
{
	std::scoped_lock lock(m_inspector->m_processRegionsMutex);

	size_t prevSearchLen = m_search.len();
	bool changed = Helpers::InputText("Search", &m_search, ImGuiInputTextFlags_AutoSelectAll);
	auto& maps = m_inspector->m_processRegions;

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
			auto& map = maps[m_filterIndices[i]];
			if (!map.m_path.contains_nocase(m_search) && !map.m_section.contains_nocase(m_search)) {
				m_filterIndices.remove(i);
			}
		}
		return;
	}

	DoSearch();
}

void MapsTab::DoSearch()
{
	m_filterIndices.clear();

	if (m_search == "") {
		return;
	}

	for (size_t i = 0; i < m_inspector->m_processRegions.len(); i++) {
		auto& map = m_inspector->m_processRegions[i];
		if (map.m_path.contains_nocase(m_search) || map.m_section.contains_nocase(m_search)) {
			m_filterIndices.add(i);
		}
	}
}
