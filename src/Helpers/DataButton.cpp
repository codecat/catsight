#include <Common.h>
#include <Helpers/DataButton.h>
#include <Inspector.h>
#include <Tabs/DataTab.h>

#include <hello_imgui.h>

void Helpers::DataButton(Inspector* inspector, uintptr_t p, int depth)
{
	static s2::list<DataTab*> _dataTabs;

	ImGui::PushID((void*)p);

	float buttonHue = fmodf(0.5f + depth * 0.05f, 1.0f);
	ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(buttonHue, 0.6f, 0.6f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(buttonHue, 0.7f, 0.7f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(buttonHue, 0.8f, 0.8f));

	s2::string buttonLabel = ICON_FA_DATABASE;
	s2::string symbolName;
	if (inspector->m_processHandle->GetSymbolName(p, symbolName)) {
		buttonLabel.appendf(" %s", symbolName.c_str());
	}

	if (ImGui::Button(buttonLabel)) {
		_dataTabs.clear();
		for (auto tab : inspector->m_tabs) {
			auto dataTab = dynamic_cast<DataTab*>(tab);
			if (dataTab != nullptr) {
				_dataTabs.add(dataTab);
			}
		}

		if (_dataTabs.len() == 0) {
			auto newTab = new DataTab(inspector, "Data", p);
			newTab->GoTo(p);
			inspector->m_tabs.add(newTab);
		} else {
			ImGui::OpenPopup("DataButtonGroup");
		}
	}

	ImGui::PopStyleColor(3);

	if (ImGui::IsItemHovered()) {
		ImGui::SetTooltip("Data pointer: " POINTER_FORMAT, p);
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
			auto newTab = new DataTab(inspector, "Data", p);
			newTab->GoTo(p);
			inspector->m_tabs.add(newTab);
		}
		ImGui::EndPopup();
	}

	ImGui::PopID();
}
