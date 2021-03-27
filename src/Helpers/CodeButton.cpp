#include <Common.h>
#include <Helpers/CodeButton.h>
#include <Inspector.h>
#include <Tabs/CodeTab.h>

#include <hello_imgui.h>

void Helpers::CodeButton(Inspector* inspector, uintptr_t p, const char* label, const char* newTabName)
{
	static s2::list<CodeTab*> _codeTabs;

	ImGui::PushID((void*)p);

	auto buttonLabel = s2::strprintf(ICON_FA_CODE " %s", label);
	if (ImGui::Button(buttonLabel)) {
		_codeTabs.clear();
		for (auto tab : inspector->m_tabs) {
			auto codeTab = dynamic_cast<CodeTab*>(tab);
			if (codeTab != nullptr) {
				_codeTabs.add(codeTab);
			}
		}

		if (_codeTabs.len() == 0) {
			auto newTab = new CodeTab(inspector, newTabName, p);
			newTab->GoTo(p);
			inspector->m_tabs.add(newTab);
		} else {
			ImGui::OpenPopup("CodeButtonPopup");
		}
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
		if (ImGui::MenuItem(ICON_FA_PLUS " New memory tab")) {
			auto newTab = new CodeTab(inspector, newTabName, p);
			newTab->GoTo(p);
			inspector->m_tabs.add(newTab);
		}
		ImGui::EndPopup();
	}

	ImGui::PopID();
}
