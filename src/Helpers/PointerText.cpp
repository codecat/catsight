#include <Common.h>
#include <Helpers/PointerText.h>
#include <Inspector.h>
#include <Resources.h>
#include <Tabs/MapsTab.h>

#include <hello_imgui.h>

void Helpers::PointerText(Inspector* inspector, uintptr_t p, const PointerMenuCallback& menuCallback)
{
	ImGui::PushID((void*)p);

	ImGui::PushFont(Resources::FontMono);
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(.5f, .5f, .5f, 1));

	ImGui::Text(POINTER_FORMAT, p);

	ImGui::PopStyleColor();
	ImGui::PopFont();

	if (ImGui::BeginPopupContextItem("pointer context menu")) {
		ImGui::TextDisabled(POINTER_FORMAT, p);

		if (ImGui::MenuItem(ICON_FA_COPY " Copy address")) {
			auto strCopyAddress = s2::strprintf(POINTER_FORMAT, p);
			ImGui::SetClipboardText(strCopyAddress);
		}

		if (ImGui::MenuItem(ICON_FA_SEARCH " Locate in maps")) {
			auto mapsTab = inspector->GetFirstTab<MapsTab>();
			if (mapsTab != nullptr) {
				mapsTab->ShowRegionPointer(p);
				mapsTab->m_shouldFocus = true;
			}
		}

		if (menuCallback != nullptr) {
			ImGui::Separator();
			menuCallback(p);
		}

		ImGui::EndPopup();
	}
	ImGui::PopID();
}
