#include <Common.h>
#include <Tabs/ModulesTab.h>
#include <Inspector.h>
#include <Resources.h>
#include <Helpers/CodeButton.h>
#include <Helpers/DataButton.h>

#include <hello_imgui.h>

ModulesTab::ModulesTab(Inspector* inspector, const s2::string& id)
	: Tab(inspector, id)
{
}

ModulesTab::~ModulesTab()
{
}

const char* ModulesTab::GetTitlePrefix()
{
	return ICON_FA_BOOK;
}

bool ModulesTab::CanClose()
{
	return false;
}

void ModulesTab::Render(float dt)
{
	if (ImGui::IsWindowAppearing()) {
		auto regions = m_inspector->m_processHandle->GetMemoryRegions();
		for (auto region : regions) {
			if (!(region.m_flags & pmrf_Image)) {
				continue;
			}

			auto existingModule = FindModule(region.m_path);
			if (existingModule == nullptr) {
				auto& newModule = m_modules.add();
				newModule.m_path = region.m_path;
				newModule.m_regions.add(region);
			} else {
				existingModule->m_regions.add(region);
			}
		}
	}

	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 0));

	if (ImGui::BeginTable("Maps", 2)) {
		ImGui::TableSetupColumn("Address", ImGuiTableColumnFlags_WidthFixed);
		ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
		ImGui::TableHeadersRow();

		ImGuiListClipper clipper;
		clipper.Begin((int)m_modules.len());
		while (clipper.Step()) {
			for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++) {
				auto& module = m_modules[i];

				ImGui::TableNextRow();

				ImGui::PushID(i);

				ImGui::TableSetColumnIndex(0);
				ImGui::PushFont(Resources::FontMono);
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(.5f, .5f, .5f, 1));
				ImGui::Text(POINTER_FORMAT, module.m_regions[0].m_start);
				ImGui::PopStyleColor();
				ImGui::PopFont();

				ImGui::TableSetColumnIndex(1);

				for (auto& region : module.m_regions) {
					if (region.IsExecute()) {
						Helpers::CodeButton(m_inspector, region.m_start);
						ImGui::SameLine();
						break;
					}
				}

				bool userModule = module.m_path.startswith(m_inspector->m_processInfo.pathDir);
				if (userModule) {
					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(.5f, 1, .5f, 1));
				}

				ImGui::Text("%s (%d regions)", module.m_path.c_str(), (int)module.m_regions.len());

				if (userModule) {
					ImGui::PopStyleColor();
				}

				ImGui::PopID();
			}
		}

		ImGui::EndTable();
	}

	ImGui::PopStyleVar();
}

ModulesTab::Module* ModulesTab::FindModule(const s2::string& path)
{
	for (auto& module : m_modules) {
		if (module.m_path == path) {
			return &module;
		}
	}
	return nullptr;
}
