#include <Common.h>
#include <Tabs/MemoryTab.h>
#include <Inspector.h>
#include <Helpers/ImGuiString.h>

#include <hello_imgui.h>

MemoryTab::MemoryTab(Inspector* inspector, const s2::string& name, uintptr_t p)
	: Tab(inspector, name)
{
	m_hasValidRegion = m_inspector->m_processHandle->GetMemoryRegion(p, m_region);

	SetRegion(m_region);
	if (p > m_region.m_start) {
		ScrollTo(p);
	}
}

MemoryTab::~MemoryTab()
{
}

void MemoryTab::SetRegion(const ProcessMemoryRegion& region)
{
	m_hasValidRegion = true;
	m_region = region;

	m_invalidated = true;
}

void MemoryTab::SetRegion(uintptr_t p)
{
	ProcessMemoryRegion region;
	if (m_inspector->m_processHandle->GetMemoryRegion(p, region)) {
		SetRegion(region);
	}
}

void MemoryTab::GoTo(uintptr_t p)
{
	if (!m_region.Contains(p)) {
		SetRegion(p);
	}
	ScrollTo(p);
}

void MemoryTab::ScrollTo(uintptr_t p)
{
	ScrollToOffset(p - m_region.m_start);
}

void MemoryTab::ScrollToOffset(uintptr_t offset)
{
	if (offset < m_region.Size()) {
		m_topOffset = offset;
		m_invalidated = true;
	}
}

void MemoryTab::RenderMenu()
{
	if (ImGui::BeginMenu("Memory")) {
		if (ImGui::MenuItem("Scroll to top", nullptr, nullptr, m_topOffset > 0)) {
			ScrollToOffset(0);
		}
		if (ImGui::MenuItem("Scroll to bottom", nullptr, nullptr, m_topOffset < m_topOffsetMax)) {
			ScrollToOffset(m_topOffsetMax);
		}

		ImGui::Separator();

		if (ImGui::MenuItem("Go to..", "Ctrl+G")) {
			m_ui_gotoPopupShow = true;
		}

		ImGui::EndMenu();
	}

	ImGui::Separator();
	ImGui::TextDisabled("Scroll: %.2f%%", (m_topOffset / (double)m_topOffsetMax) * 100.0);
}

bool MemoryTab::RenderBegin()
{
	if (ImGui::IsWindowAppearing()) {
		m_invalidated = true;
	}

	if (!m_hasValidRegion) {
		ImGui::TextUnformatted("No valid region");
		return false;
	}

	if (ImGui::GetIO().KeyCtrl && ImGui::IsKeyPressed('G')) {
		m_ui_gotoPopupShow = true;
	}
	if (m_ui_gotoPopupShow) {
		ImGui::OpenPopup("GoToPopup");
		m_ui_gotoPopupShow = false;
	}
	if (ImGui::BeginPopup("GoToPopup")) {
		if (ImGui::IsWindowAppearing()) {
			ImGui::SetKeyboardFocusHere();
		}
		bool actuallyGo = Helpers::InputText("Address", &m_ui_gotoAddressString, ImGuiInputTextFlags_EnterReturnsTrue);

		uintptr_t gotoPointer = 0;
		if (m_ui_gotoAddressString.len() > 0) {
			//TODO: This could be parser as an expression instead (with numbers being hex w/o 0x prefix)
			sscanf(m_ui_gotoAddressString, "%llx", &gotoPointer);

			bool valid = m_inspector->m_processHandle->IsReadableMemory(gotoPointer);
			if (!valid) {
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, .5f, .5f, 1));
			}
			ImGui::Text(POINTER_FORMAT, gotoPointer);
			if (!valid) {
				ImGui::PopStyleColor();
			}
		}

		if (actuallyGo) {
			printf("Goto: %s\n", m_ui_gotoAddressString.c_str());
			GoTo(gotoPointer);
			m_ui_gotoAddressString = "";
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}

	ImGui::BeginChild("Memory", ImVec2(), false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 0));

	auto& style = ImGui::GetStyle();
	auto windowSize = ImGui::GetWindowSize();
	auto startPos = ImGui::GetCursorPos();

	int itemsPerPage = windowSize.y / m_itemHeight;
	if (m_itemsPerPage != itemsPerPage) {
		m_itemsPerPage = itemsPerPage;
		m_invalidated = true;
	}

	//NOTE: This assumes that pages are always aligned. Is that always the case though?
	m_topOffsetMax = m_region.Size() - m_itemsPerPage * sizeof(uintptr_t);

	if (ImGui::IsWindowHovered()) {
		float mouseWheel = ImGui::GetIO().MouseWheel;
		if (mouseWheel != 0) {
			// mouseWheel: negative = scroll down, positive = scroll up
			m_topOffset += GetScrollAmount((int)mouseWheel);
			m_invalidated = true;
		}
	}

	// Virtual scrollbar
	if (m_showScrollBar) {
		ImGui::SetCursorPos(ImVec2(startPos.x + windowSize.x - style.ScrollbarSize, startPos.y));
		intptr_t virtualScrollPos = m_topOffset & ~0x7;
		intptr_t virtualScrollMin = 0;
		if (ImGui::VSliderScalar("", ImVec2(style.ScrollbarSize, windowSize.y), ImGuiDataType_S64, &virtualScrollPos, &m_topOffsetMax, &virtualScrollMin, "", ImGuiSliderFlags_NoInput)) {
			m_topOffset = virtualScrollPos & ~0x7;
			m_invalidated = true;
		}
		ImGui::SetCursorPos(startPos);
	}

	// Constrain scroll offset
	if (m_topOffset < 0) {
		m_topOffset = 0;
		m_invalidated = true;
	} else if (m_topOffset > m_topOffsetMax) {
		m_topOffset = m_topOffsetMax;
		m_invalidated = true;
	}

	return true;
}

void MemoryTab::RenderEnd()
{
	if (m_invalidated) {
		m_invalidated = false;
	}

	ImGui::PopStyleVar();
	ImGui::EndChild();
}

intptr_t MemoryTab::GetScrollAmount(int wheel)
{
	return (wheel * -1) * 3 * sizeof(uintptr_t);
}

const char* MemoryTab::DetectString(uintptr_t p)
{
	auto handle = m_inspector->m_processHandle;

	// It might be a string if:
	// - The relative offset is 0
	// - The pointer is not 0
	// - The pointer is valid and can be read
	// - There are at least 5 printable characters

	if (p == 0) {
		return nullptr;
	}

	if (!handle->IsReadableMemory(p)) {
		return nullptr;
	}

	static s2::string _stringBuffer;

	for (int i = 0; i < 5; i++) {
		char c = handle->Read<char>(p + i);
		if (c < 0x20 || c > 0x7E) {
			break;
		}
		if (i == 4) {
			_stringBuffer = handle->ReadCString(p)
				.replace("\t", "\\t")
				.replace("\r", "\\r")
				.replace("\n", "\\n")
			;
			return _stringBuffer.c_str();
		}
	}

	return nullptr;
}
