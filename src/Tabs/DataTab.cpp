#include <Common.h>
#include <Tabs/DataTab.h>

#include <Inspector.h>
#include <Resources.h>
#include <Helpers/DataButton.h>
#include <Helpers/CodeButton.h>

#include <hello_imgui.h>

DataTab::DataTab(Inspector* inspector, const s2::string& name, uintptr_t p)
	: MemoryTab(inspector, name, p)
{
	m_addressMask = 0x7;
}

DataTab::~DataTab()
{
}

uint16_t DataTab::RenderMember(uintptr_t offset, uint16_t relativeOffset, intptr_t displayOffset, int lineIndex)
{
	assert(lineIndex < (int)m_lineDetails.len());
	auto& line = m_lineDetails[lineIndex];

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

	// It might be a string:
	if (relativeOffset == 0) {
		const char* str = DetectString(p);
		if (str != nullptr) {
			ImGui::SameLine();
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, .5f, 1));
			ImGui::Text("\"%s\"", str);
			ImGui::PopStyleColor();
			return sizeof(uintptr_t);
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
		if (m_invalidated) {
			line.m_memoryExecutable = false;
			line.m_pointsToExecutable = false;

			ProcessMemoryRegion region;
			if (handle->GetMemoryRegion(p, region)) {
				line.m_memoryExecutable = region.IsExecute();
			}

			auto pp = handle->Read<uintptr_t>(p);
			if (handle->IsReadableMemory(pp) && handle->GetMemoryRegion(pp, region) && region.IsExecute()) {
				line.m_pointsToExecutable = true;
				line.m_pointsToExecutableValue = pp;
			}
		}

		ImGui::SameLine();

		if (line.m_memoryExecutable) {
			Helpers::CodeButton(m_inspector, p);
			ImGui::SameLine();
		}

		if (line.m_pointsToExecutable) {
			Helpers::CodeButton(m_inspector, line.m_pointsToExecutableValue);
			ImGui::SameLine();
		}

		// We can't do anything else with arbitrary pointers besides show a memory button
		if (pointerIsAligned) {
			Helpers::DataButton(m_inspector, p);
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

s2::string DataTab::GetLabel()
{
	return s2::strprintf(ICON_FA_DATABASE " %s (" POINTER_FORMAT ")###Memory", MemoryTab::GetLabel().c_str(), m_region.m_start + m_baseOffset);
}

void DataTab::RenderMenu()
{
	if (ImGui::BeginMenu("Data")) {
		if (ImGui::MenuItem("Reset base offset", nullptr, nullptr, m_baseOffset > 0)) {
			m_baseOffset = 0;
		}
		if (ImGui::MenuItem("Reset base size", nullptr, nullptr, m_baseSize > 0)) {
			m_baseSize = 0;
		}
		ImGui::EndMenu();
	}

	MemoryTab::RenderMenu();
}

void DataTab::Render()
{
	m_lineDetails.ensure_memory(m_itemsPerPage + 1);
	while (m_lineDetails.len() < m_itemsPerPage + 1) {
		m_lineDetails.add();
	}

	size_t base = m_region.m_start;
	size_t size = m_region.m_end - m_region.m_start;

	uint16_t currentMemberSize = 0;

	for (int i = 0; i < m_itemsPerPage + 1; i++) {
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

		column += 70;
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
}
