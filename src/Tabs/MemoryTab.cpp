#include <Common.h>
#include <Tabs/MemoryTab.h>

#include <Inspector.h>
#include <Resources.h>
#include <Helpers/MemoryButton.h>

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

void MemoryTab::SetRegion(const ProcessMemoryRegion& region, uintptr_t baseOffset, uintptr_t baseSize)
{
	m_hasValidRegion = true;
	m_region = region;

	if (baseOffset < m_region.Size()) {
		m_baseOffset = baseOffset;
	}
	m_baseSize = baseSize;

	m_invalidated = true;
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
		m_topOffset = offset;
		m_invalidated = true;
	}
}

uint16_t MemoryTab::RenderMember(uintptr_t offset, uint16_t relativeOffset, intptr_t displayOffset, int lineIndex)
{
	assert(lineIndex < (int)m_lineDetails.len());
	auto& lineDetails = m_lineDetails[lineIndex];
	bool lineAppearing = m_invalidated;

	offset += relativeOffset;
	uintptr_t address = (uintptr_t)m_region.m_start + offset;

	// Shorthand for process handle
	auto handle = m_inspector->m_processHandle;

	// Prepare some types derived from the value at the address
	uintptr_t p = handle->Read<uintptr_t>(address);
	uint32_t u32 = handle->Read<uint32_t>(address);
	float f = handle->Read<float>(address);

#if defined(PLATFORM_64)
	// 64 bit pointers are typically aligned to 16 bytes
	//TODO: Make an option to ignore this check
	bool pointerIsAligned = (p & 0xF) == 0;
#else
	// 32 bit targets don't care about alignment
	bool pointerIsAligned = true;
#endif
	const bool resolveFloats = false; //TODO: Option for this

	// NOTE: The order of which these are checked is important! We should test for the least common types first!

	//TODO: Allow plugins to detect stuff here

	// It might be a string if:
	// - The relative offset is 0
	// - The pointer is not 0
	// - The pointer is valid and can be read
	// - There are at least 5 printable characters
	if (relativeOffset == 0 && p != 0 && handle->IsReadableMemory(p)) {
		for (int i = 0; i < 5; i++) {
			char c = handle->Read<char>(p + i);
			if (c < 0x20 || c > 0x7E) {
				break;
			}
			if (i == 4) {
				m_stringBuffer = handle->ReadCString(p);

				ImGui::SameLine();
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, .5f, .5f, 1));
				ImGui::Text("\"%s\"", m_stringBuffer.c_str());
				ImGui::PopStyleColor();
				return sizeof(uintptr_t);
			}
		}
	}

	// It might be a wide string if:
	// - The relative offset is 0
	// - The pointer is not 0
	// - The pointer is valid and can be read
	// - There are at least 5 printable wide characters
	//TODO: Fix this
	/*
	if (relativeOffset == 0 && p != 0 && handle->IsReadableMemory(p)) {
		for (int i = 0; i < 5; i++) {
			wchar_t c = handle->Read<wchar_t>(p + i);
			if (c < 0x20 || c > 0x7E) {
				break;
			}
			if (i == 4) {
				ImGui::SameLine();
				//TODO: Render wide string
			}
		}
	}
	*/

	if (resolveFloats) {
		// It might be a float if:
		// - The 32 bit integer is not 0
		// - The float is not NaN
		// - It's larger than or equal to 0.0001f
		// - It's smaller than or equal to 100000.0f
		if (u32 != 0 && !std::isnan(f) && !std::isinf(f) && fabsf(f) >= 0.0001f && fabsf(f) <= 100000.0f) {
			ImGui::SameLine();
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, .5f, 1));
			ImGui::Text("%g", f);
			ImGui::PopStyleColor();
			return sizeof(float);
		}
	}

	// It might be an arbitrary pointer if:
	// - The relative offset is 0
	// - The pointer is not 0
	// - The pointer is aligned
	// - The pointer is valid and can be read
	if (relativeOffset == 0 && p != 0 && (p & 0xFFFFF0) != 0 && handle->IsReadableMemory(p)) {
		if (lineAppearing) {
			ProcessMemoryRegion region;
			if (handle->GetMemoryRegion(p, region)) {
				lineDetails.m_memoryExecutable = region.IsExecute();
			}
		}

		ImGui::SameLine();

		if (lineDetails.m_memoryExecutable) {
			ImGui::TextDisabled("(executable)");
			ImGui::SameLine();
		}

		// We can't do anything else with arbitrary pointers besides show a memory button
		if (pointerIsAligned) {
			Helpers::MemoryButton(m_inspector, p, "Memory");
		}
		return sizeof(uintptr_t);
	}

	//TODO: We can't display this at the end!
	/*
	if (renderFound && relativeOffset > 0) {
		ImGui::SameLine();
		if (displayOffset < 0) {
			ImGui::Text("$\\$f77-%X", abs(displayOffset));
		} else {
			ImGui::Text("$\\$f77+%X", displayOffset);
		}
	}
	*/

	return 0;
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

		ImGui::Separator();

		if (ImGui::MenuItem("Scroll to top", nullptr, nullptr, m_topOffset > 0)) {
			ScrollToOffset(0);
		}
		if (ImGui::MenuItem("Scroll to bottom", nullptr, nullptr, m_topOffset < m_topOffsetMax)) {
			ScrollToOffset(m_topOffsetMax);
		}

		ImGui::EndMenu();
	}

	ImGui::Separator();
	ImGui::TextDisabled("Scroll: %.2f%%", (m_topOffset / (double)m_topOffsetMax) * 100.0);
}

void MemoryTab::Render()
{
	if (!m_hasValidRegion) {
		ImGui::TextUnformatted("No valid region");
		return;
	}

	ImGui::BeginChild("Memory", ImVec2(), false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

	auto& style = ImGui::GetStyle();
	auto windowSize = ImGui::GetWindowSize();
	auto startPos = ImGui::GetCursorPos();

	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 0));

	size_t base = m_region.m_start;
	size_t size = m_region.m_end - m_region.m_start;

	uint16_t currentMemberSize = 0;

	const float itemHeight = 22.0f;
	const int itemsPerPage = windowSize.y / itemHeight;

	m_lineDetails.ensure_memory(itemsPerPage + 1);
	while (m_lineDetails.len() < itemsPerPage + 1) {
		m_lineDetails.add();
	}

	//NOTE: This assumes that pages are always aligned. Is that always the case though?
	m_topOffsetMax = m_region.Size() - itemsPerPage * sizeof(uintptr_t);

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

	for (int i = 0; i < itemsPerPage + 1; i++) {
		uintptr_t offset = (uintptr_t)m_topOffset + i * sizeof(uintptr_t);
		uintptr_t address = base + offset;

		if (!m_inspector->m_processHandle->IsReadableMemory(address)) {
			break;
		}

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

		while (currentMemberSize < sizeof(uintptr_t)) {
			uint16_t size = RenderMember(offset, currentMemberSize, displayOffset, i);
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

		ImGui::NewLine();

		ImGui::PopID();
	}

	if (m_invalidated) {
		m_invalidated = false;
	}

	ImGui::PopStyleVar();
	ImGui::EndChild();
}
