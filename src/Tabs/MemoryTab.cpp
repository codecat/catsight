#include <Common.h>
#include <Tabs/MemoryTab.h>

#include <Inspector.h>
#include <Resources.h>

#include <hello_imgui.h>

#if defined(PLATFORM_64)
#  define POINTER_FORMAT "%016llX"
#  define OFFSET_FORMAT "%llX"
#else
#  define POINTER_FORMAT "%08X"
#  define OFFSET_FORMAT "%X"
#endif

MemoryTab::MemoryTab(Inspector* inspector, const s2::string& name, uintptr_t p)
	: Tab(inspector, name)
{
	m_hasValidRegion = m_inspector->m_processHandle->GetMemoryRegion(p, m_region);
}

MemoryTab::~MemoryTab()
{
}

void MemoryTab::Render()
{
	if (!m_hasValidRegion) {
		ImGui::TextUnformatted("No valid region");
		return;
	}

	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 0));

	size_t base = m_region.m_start;
	size_t size = m_region.m_end - m_region.m_start;

	uint16_t currentMemberSize = 0;

	ImGuiListClipper clipper;
	clipper.Begin((int)(size / sizeof(uintptr_t)));
	while (clipper.Step()) {
		for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++) {
			uintptr_t offset = (uintptr_t)(i * sizeof(uintptr_t));
			intptr_t displayOffset = (intptr_t)offset - (intptr_t)m_baseOffset;
			uintptr_t address = base + offset;

			if (offset + sizeof(uintptr_t) > size) {
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 0, 0, 1));
				ImGui::Text("Doesn't fit (TODO!)");
				ImGui::PopStyleColor();
				continue;
			}

			ImGui::PushID((void*)address);

			float column = 0.0f;

			ImGui::PushFont(Resources::FontMono);
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(.5f, .5f, .5f, 1));
			ImGui::Text(POINTER_FORMAT, address);
			ImGui::PopStyleColor();
			ImGui::PopFont();
			ImGui::SameLine();

			column += 130;

			if (ImGui::Button("$")) {
				m_baseOffset = offset;
			}
			ImGui::SameLine();

			if (displayOffset == 0) {
				ImGui::TextUnformatted("$ ==>");
			} else {
				const char* format = "$+" OFFSET_FORMAT;
				if (displayOffset < 0) {
					format = "$-" OFFSET_FORMAT;
				}
				const int depth = 0;
				uintptr_t absDisplayOffset = (uintptr_t)(displayOffset < 0 ? displayOffset * -1 : displayOffset);
				ImGui::TextColored(ImColor::HSV(fmodf((float)depth * 0.15f, 1.0f), 0.65f, 1.0f), format, absDisplayOffset);
			}

			ImGui::SameLine(column + 70.0f);

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

			ImGui::SameLine(column + 200.0f);

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

	ImGui::PopStyleVar();
}
