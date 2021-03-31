#include <Common.h>
#include <Helpers/PointerText.h>
#include <Inspector.h>
#include <Resources.h>
#include <Tabs/MapsTab.h>

#include <hello_imgui.h>

void Helpers::PointerText(Inspector* inspector, uintptr_t p)
{
	ImGui::PushID((void*)p);

	ImGui::PushFont(Resources::FontMono);
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(.5f, .5f, .5f, 1));

	ImGui::Text(POINTER_FORMAT, p);

	ImGui::PopStyleColor();
	ImGui::PopFont();

	if (ImGui::BeginPopupContextItem("pointer context menu")) {
		if (ImGui::MenuItem("Locate in maps")) {
			auto mapsTab = inspector->GetFirstTab<MapsTab>();
			if (mapsTab != nullptr) {
				mapsTab->ShowRegionPointer(p);
				mapsTab->m_shouldFocus = true;
			}
		}
		ImGui::EndPopup();
	}
	ImGui::PopID();
}
