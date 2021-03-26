#include <Common.h>
#include <Helpers/MemoryButton.h>
#include <Inspector.h>
#include <Tabs/MemoryTab.h>

#include <hello_imgui.h>

void Helpers::MemoryButton(Inspector* inspector, uintptr_t p, const char* label, const char* newTabName)
{
	static s2::list<MemoryTab*> _memoryTabs;

	if (ImGui::Button(label)) {
		_memoryTabs.clear();
		for (auto tab : inspector->m_tabs) {
			auto memoryTab = dynamic_cast<MemoryTab*>(tab);
			if (memoryTab != nullptr) {
				_memoryTabs.add(memoryTab);
			}
		}

		if (_memoryTabs.len() == 0) {
			auto newTab = new MemoryTab(inspector, newTabName, p);
			newTab->GoTo(p);
			inspector->m_tabs.add(newTab);
		} else {
			ImGui::OpenPopup("MemoryButtonPopup");
		}
	}

	if (ImGui::BeginPopup("MemoryButtonPopup")) {
		for (auto tab : _memoryTabs) {
			ImGui::PushID(tab);
			if (ImGui::MenuItem(tab->GetLabel())) {
				tab->GoTo(p);
				tab->m_shouldFocus = true;
			}
			ImGui::PopID();
		}
		ImGui::Separator();
		if (ImGui::MenuItem(ICON_FA_PLUS " New memory tab")) {
			auto newTab = new MemoryTab(inspector, newTabName, p);
			newTab->GoTo(p);
			inspector->m_tabs.add(newTab);
		}
		ImGui::EndPopup();
	}
}
