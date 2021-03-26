#include <Common.h>
#include <Tabs/MemoryTab.h>

#include <Inspector.h>
#include <Resources.h>

#include <hello_imgui.h>

MemoryTab::MemoryTab(Inspector* inspector, const s2::string& name, uintptr_t p)
	: Tab(inspector, name)
{
	m_hasValidRegion = m_inspector->m_processHandle->GetMemoryRegion(p, m_region);
	if (p > m_region.m_start) {
		ScrollTo(p);
	}
}

MemoryTab::~MemoryTab()
{
}

void MemoryTab::SetRegion(const ProcessMemoryRegion& region, uintptr_t baseOffset, uintptr_t baseSize)
{
	m_hasValidRegion = true;
	m_region = region;

	if (baseOffset < m_region.Size()) {
		m_baseOffset = baseOffset;
	}
	m_baseSize = baseSize;
}

void MemoryTab::SetRegion(uintptr_t p, uintptr_t baseOffset, uintptr_t baseSize)
{
	ProcessMemoryRegion region;
	if (m_inspector->m_processHandle->GetMemoryRegion(p, region)) {
		SetRegion(region, baseOffset, baseSize);
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
		m_scrollToOffset = offset;
	}
}

s2::string MemoryTab::GetLabel()
{
	return s2::strprintf("%s (" POINTER_FORMAT ")###Memory", Tab::GetLabel().c_str(), m_region.m_start + m_baseOffset);
}

void MemoryTab::RenderMenu()
{
	if (ImGui::BeginMenu("Memory")) {
		if (ImGui::MenuItem("Reset base offset", nullptr, nullptr, m_baseOffset > 0)) {
			m_baseOffset = 0;
		}
		if (ImGui::MenuItem("Reset base size", nullptr, nullptr, m_baseSize > 0)) {
			m_baseSize = 0;
		}

		ImGui::EndMenu();
	}
}

void MemoryTab::Render()
{
	if (!m_hasValidRegion) {
		ImGui::TextUnformatted("No valid region");
		return;
	}

	ImGui::BeginChild("Memory");
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 0));

	size_t base = m_region.m_start;
	size_t size = m_region.m_end - m_region.m_start;

	uint16_t currentMemberSize = 0;

	ImGuiListClipper clipper;
	clipper.Begin((int)(size / sizeof(uintptr_t)));
	while (clipper.Step()) {
		for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++) {
			uintptr_t offset = (uintptr_t)(i * sizeof(uintptr_t));
			uintptr_t address = base + offset;

			intptr_t relativeOffset = (intptr_t)offset - (intptr_t)m_baseOffset;

			intptr_t displayOffset, displayOffsetDepth;
			if (m_baseSize == 0) {
				displayOffset = relativeOffset;
				displayOffsetDepth = 0;
			} else {
				displayOffset = relativeOffset % (intptr_t)m_baseSize;
				displayOffsetDepth = relativeOffset / (intptr_t)m_baseSize;

				if (relativeOffset < 0) {
					displayOffset = m_baseSize + displayOffset - sizeof(uintptr_t);
					displayOffsetDepth *= -1;
					displayOffsetDepth++;
				}
			}

			if (offset + sizeof(uintptr_t) > size) {
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 0, 0, 1));
				ImGui::Text("Doesn't fit (TODO!)");
				ImGui::PopStyleColor();
				continue;
			}

			ImGui::PushID((void*)address);
			ImGui::PushFont(Resources::FontMono);

			float column = 0.0f;

			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(.5f, .5f, .5f, 1));
			ImGui::Text(POINTER_FORMAT, address);
			ImGui::PopStyleColor();
			ImGui::SameLine();

			column += 130;

			if (ImGui::Button("$")) {
				m_baseOffset = offset;
			}
			ImGui::SameLine();
			if (relativeOffset > 0 && ImGui::Button(">")) {
				if (m_baseSize == relativeOffset) {
					m_baseSize = 0;
				} else {
					m_baseSize = relativeOffset;
				}
			}

			column += 60;
			ImGui::SameLine(column);

			ImGui::PushStyleColor(ImGuiCol_Text, ImColor::HSV(fmodf((float)displayOffsetDepth * 0.15f, 1.0f), 0.65f, 1.0f).Value);
			if (displayOffset == 0) {
				ImGui::TextUnformatted("$ ==>");
			} else {
				const char* format = "$+" OFFSET_FORMAT;
				if (displayOffset < 0) {
					format = "$-" OFFSET_FORMAT;
				}
				uintptr_t absDisplayOffset = (uintptr_t)(displayOffset < 0 ? displayOffset * -1 : displayOffset);
				ImGui::Text(format, absDisplayOffset);
			}
			ImGui::PopStyleColor();
			ImGui::PopFont();

			column += 100;
			ImGui::SameLine(column);

			uintptr_t value = 0;
			if (m_inspector->m_processHandle->ReadMemory(address, &value, sizeof(value)) == sizeof(value)) {
				ImGui::PushFont(Resources::FontMono);
				if (value == 0) {
					ImGui::TextDisabled(POINTER_FORMAT, value);
				} else {
					ImGui::Text(POINTER_FORMAT, value);
				}
				ImGui::PopFont();
			}

			column += 120;
			ImGui::SameLine(column);

#if 0
			while (currentMemberSize < sizeof(uintptr_t)) {
				uint16_t size = RenderMember(tab, offset, currentMemberSize);
				if (size == 0) {
					// On 32 bit, advancing 4 bytes goes to the next row (eg. every possible item)
					// On 64 bit, advancing 4 bytes goes to the next possible item (eg. float, int32, etc)
					size = 4;
				}
				currentMemberSize += size;
				ImGui::SameLine();
			}

			// Slowly reducing bytes allows us to gradually interpret data over multiple rows (eg. a vec3
			// has 12 bytes which spans 2 lines on 64 bit and 3 lines on 32 bit), which makes it so that
			// data isn't interpreted until the end of the previously interpreted data. This might be a
			// little bit buggy when items are partly on screen due to the fact we're using ImGuiListClipper.
			if (currentMemberSize > 0) {
				currentMemberSize -= (currentMemberSize >= sizeof(uintptr_t) ? sizeof(uintptr_t) : currentMemberSize);
			}
#endif

			ImGui::NewLine();

			ImGui::PopID();
		}
	}

	if (m_scrollToOffset != -1) {
		ImGui::SetScrollFromPosY(ImGui::GetCursorStartPos().y + m_scrollToOffset / sizeof(uintptr_t) * clipper.ItemsHeight, 0.0f);
		m_scrollToOffset = -1;
	}

	ImGui::PopStyleVar();
	ImGui::EndChild();
}
