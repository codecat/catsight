#include <Common.h>
#include <Tabs/MemoryTab.h>
#include <Inspector.h>
#include <Helpers/MemoryValidator.h>
#include <Helpers/CodeButton.h>
#include <Helpers/DataButton.h>
#include <Helpers/ImGuiString.h>
#include <Helpers/Expression.h>

#include <hello_imgui.h>

MemoryTab::MemoryTab(Inspector* inspector, const s2::string& name, uintptr_t p)
	: Tab(inspector, name)
	, m_typeRenderer(inspector)
{
	GoTo(p);
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
	if (m_inspector->GetMemoryRegion(p, region)) {
		SetRegion(region);
	} else {
		m_hasValidRegion = false;
	}
}

void MemoryTab::GoTo(uintptr_t p, bool addToHistory)
{
	// Don't have to do anything if we're already there
	if (m_hasValidRegion && p == m_region.m_start + m_topOffset) {
		return;
	}

	if (!m_region.Contains(p)) {
		SetRegion(p);
	}
	ScrollTo(p);

	if (addToHistory) {
		AddHistory();
	}
}

void MemoryTab::AddHistory()
{
	// Remove all history items after the current index
	while ((int)m_history.len() > m_historyIndex + 1) {
		m_history.remove(m_history.len() - 1);
	}

	// Add a new history item
	m_history.add(m_region.m_start + m_topOffset);
	m_historyIndex++;

	// Make sure we don't have too many items
	LimitHistorySize();
}

void MemoryTab::LimitHistorySize()
{
	// Remove history items from the bottom of the stack until we satisfy the limit
	while (m_history.len() > m_maxHistoryItems) {
		m_history.remove(0);
		m_historyIndex--;
	}
}

void MemoryTab::GoToHistory(int offset)
{
	// Add to the current index
	int newIndex = m_historyIndex + offset;

	// Clamp new index to boundaries
	if (newIndex < 0) {
		newIndex = 0;
	} else if (newIndex >= (int)m_history.len()) {
		newIndex = (int)m_history.len() - 1;
	}

	// Don't have to do anything if the history index didn't actually change
	if (newIndex == m_historyIndex) {
		return;
	}
	m_historyIndex = newIndex;

	// Go to the pointer without adding it to the history
	GoTo(m_history[m_historyIndex], false);
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

void MemoryTab::RenderMenu(float dt)
{
	if (ImGui::BeginMenu("View")) {
		if (ImGui::MenuItem("Scroll to top", nullptr, nullptr, m_topOffset > 0)) {
			ScrollToOffset(0);
		}
		if (ImGui::MenuItem("Scroll to bottom", nullptr, nullptr, m_topOffset < m_topOffsetMax)) {
			ScrollToOffset(m_topOffsetMax);
		}

		ImGui::Separator();

		if (ImGui::MenuItem("Previous", nullptr, nullptr, m_historyIndex > 0)) {
			GoToHistory(-1);
		}
		if (ImGui::MenuItem("Next", nullptr, nullptr, m_historyIndex < (int)m_history.len() - 1)) {
			GoToHistory(1);
		}
		if (ImGui::BeginMenu("History", m_history.len() > 0)) {
			for (int i = 0; i < (int)m_history.len(); i++) {
				auto p = m_history[i];
				auto strItem = s2::strprintf("%d: " POINTER_FORMAT, i, p);
				if (ImGui::MenuItem(strItem, nullptr, m_historyIndex == i)) {
					GoTo(p, false);
					m_historyIndex = i;
				}
			}
			ImGui::EndMenu();
		}

		ImGui::Separator();

		if (ImGui::MenuItem("Go to..", "Ctrl+G")) {
			m_ui_gotoPopupShow = true;
		}

		ImGui::EndMenu();
	}

	if (m_typeRenderer.RenderMenu()) {
		m_invalidated = true;
	}

	ImGui::Separator();
	ImGui::TextDisabled("Scroll: %.2f%%", (m_topOffset / (double)m_topOffsetMax) * 100.0);
}

bool MemoryTab::RenderBegin(float dt)
{
	// Invalidate if we're switching to this tab
	if (ImGui::IsWindowAppearing()) {
		m_invalidated = true;
	}

	// Stop if we don't have a valid region
	if (!m_hasValidRegion) {
		ImGui::TextUnformatted("No valid region");
		return false;
	}

	// Go To dialog (Ctrl+G)
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
			gotoPointer = Helpers::EvaluateExpression(m_ui_gotoAddressString);

			bool valid = m_inspector->m_processHandle->IsReadableMemory(gotoPointer);
			if (!valid) {
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, .5f, .5f, 1));
			}
			ImGui::Text(POINTER_FORMAT, gotoPointer);
			if (!valid) {
				ImGui::PopStyleColor();
			}

			//TODO: Try to resolve and render value of pointer here
		}

		if (actuallyGo) {
			GoTo(gotoPointer);
			m_ui_gotoAddressString = "";
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}

	// Handle mouse previous and next buttons to cycle through history
	if (ImGui::IsMouseClicked(3)) {
		GoToHistory(-1);
	}
	if (ImGui::IsMouseClicked(4)) {
		GoToHistory(1);
	}

	// Begin memory view
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

	// Remember if we were invalidated at the beginning of the tab or not
	m_wasInvalidatedAtBegin = m_invalidated;

	return true;
}

void MemoryTab::RenderEnd(float dt)
{
	if (m_invalidated) {
		// If we were not invalidated at the beginning but we are now, then we were invalidated in the middle of drawing the tab.
		// In this case, we cannot guarantee a 100% valid tab, so we keep the tab invalidated for another frame.
		if (m_wasInvalidatedAtBegin) {
			m_invalidated = false;
		}
	}

	ImGui::PopStyleVar();
	ImGui::EndChild();
}

intptr_t MemoryTab::GetScrollAmount(int wheel)
{
	return (wheel * -1) * 3 * sizeof(uintptr_t);
}
