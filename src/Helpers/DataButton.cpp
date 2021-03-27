#include <Common.h>
#include <Helpers/DataButton.h>
#include <Inspector.h>
#include <Tabs/DataTab.h>

#include <hello_imgui.h>

void Helpers::DataButton(Inspector* inspector, uintptr_t p, const char* label, const char* newTabName)
{
	static s2::list<DataTab*> _dataTabs;

	ImGui::PushID((void*)p);

	if (ImGui::Button(label)) {
		_dataTabs.clear();
		for (auto tab : inspector->m_tabs) {
			auto dataTab = dynamic_cast<DataTab*>(tab);
			if (dataTab != nullptr) {
				_dataTabs.add(dataTab);
			}
		}

		if (_dataTabs.len() == 0) {
			auto newTab = new DataTab(inspector, newTabName, p);
			newTab->GoTo(p);
			inspector->m_tabs.add(newTab);
		} else {
			ImGui::OpenPopup("DataButtonGroup");
		}
	}

	if (ImGui::BeginPopup("DataButtonGroup")) {
		ImGui::TextDisabled(POINTER_FORMAT, p);
		for (auto tab : _dataTabs) {
			ImGui::PushID(tab);
			if (ImGui::MenuItem(tab->GetLabel())) {
				tab->GoTo(p);
				tab->m_shouldFocus = true;
			}
			ImGui::PopID();
		}
		ImGui::Separator();
		if (ImGui::MenuItem(ICON_FA_PLUS " New data tab")) {
			auto newTab = new DataTab(inspector, newTabName, p);
			newTab->GoTo(p);
			inspector->m_tabs.add(newTab);
		}
		ImGui::EndPopup();
	}

	ImGui::PopID();
}
