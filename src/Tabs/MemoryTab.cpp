#include <Common.h>
#include <Tabs/MemoryTab.h>
#include <Inspector.h>

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

	ImGui::BeginChild("Memory", ImVec2(), false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 0));

	auto& style = ImGui::GetStyle();
	auto windowSize = ImGui::GetWindowSize();
	auto startPos = ImGui::GetCursorPos();

	m_itemsPerPage = windowSize.y / m_itemHeight;

	//NOTE: This assumes that pages are always aligned. Is that always the case though?
	m_topOffsetMax = m_region.Size() - m_itemsPerPage * sizeof(uintptr_t);

	if (ImGui::IsWindowHovered()) {
		float mouseWheel = ImGui::GetIO().MouseWheel;
		if (mouseWheel != 0) {
			// mouseWheel: negative = scroll down, positive = scroll up
			m_topOffset += ((int)mouseWheel * -1) * 4 * sizeof(uintptr_t);
			m_invalidated = true;
		}
	}

	// Virtual scrollbar
	ImGui::SetCursorPos(ImVec2(startPos.x + windowSize.x - style.ScrollbarSize, startPos.y));
	intptr_t virtualScrollPos = m_topOffset & ~0x7;
	intptr_t virtualScrollMin = 0;
	if (ImGui::VSliderScalar("", ImVec2(style.ScrollbarSize, windowSize.y), ImGuiDataType_S64, &virtualScrollPos, &m_topOffsetMax, &virtualScrollMin, "", ImGuiSliderFlags_NoInput)) {
		m_topOffset = virtualScrollPos & ~0x7;
		m_invalidated = true;
	}
	ImGui::SetCursorPos(startPos);

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
