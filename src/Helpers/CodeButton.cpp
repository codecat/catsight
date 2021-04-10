#include <Common.h>
#include <Helpers/CodeButton.h>
#include <Inspector.h>
#include <Tabs/CodeTab.h>

#include <hello_imgui.h>

void Helpers::CodeButton(Inspector* inspector, uintptr_t p, int depth)
{
	static s2::list<CodeTab*> _codeTabs;

	ImGui::PushID((void*)p);

	float buttonHue = fmodf(depth * 0.05f, 1.0f);
	ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(buttonHue, 0.6f, 0.6f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(buttonHue, 0.7f, 0.7f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(buttonHue, 0.8f, 0.8f));

	s2::string buttonLabel = ICON_FA_CODE_BRANCH;

	s2::string name;
	if (inspector->m_labels.GetLabel(p, name)) {
		buttonLabel.appendf(" %s", name.c_str());
	} else if (inspector->m_processHandle->GetSymbolName(p, name)) {
		buttonLabel.appendf(" %s", name.c_str());
	}

	if (ImGui::Button(buttonLabel)) {
		_codeTabs.clear();
		for (auto tab : inspector->m_tabs) {
			auto codeTab = dynamic_cast<CodeTab*>(tab);
			if (codeTab != nullptr) {
				_codeTabs.add(codeTab);
			}
		}

		if (_codeTabs.len() == 0) {
			auto newTab = new CodeTab(inspector, "Code", p);
			newTab->GoTo(p);
			inspector->m_tabs.add(newTab);
		} else {
			ImGui::OpenPopup("CodeButtonPopup");
		}
	}

	ImGui::PopStyleColor(3);

	if (ImGui::IsItemHovered()) {
		ImGui::SetTooltip("Code pointer: " POINTER_FORMAT, p);
	}

	if (ImGui::BeginPopup("CodeButtonPopup")) {
		ImGui::TextDisabled(POINTER_FORMAT, p);
		for (auto tab : _codeTabs) {
			ImGui::PushID(tab);
			if (ImGui::MenuItem(tab->GetLabel())) {
				tab->GoTo(p);
				tab->m_shouldFocus = true;
			}
			ImGui::PopID();
		}
		ImGui::Separator();
		if (ImGui::MenuItem(ICON_FA_PLUS " New code tab")) {
			auto newTab = new CodeTab(inspector, "Code", p);
			newTab->GoTo(p);
			inspector->m_tabs.add(newTab);
		}
		ImGui::EndPopup();
	}

	ImGui::PopID();
}
