#include <Common.h>
#include <Tabs/DataTab.h>

#include <Inspector.h>
#include <Resources.h>
#include <Helpers/DataButton.h>
#include <Helpers/CodeButton.h>

#include <hello_imgui.h>

DataTab::DataTab(Inspector* inspector, const s2::string& id, uintptr_t p)
	: MemoryTab(inspector, id, p)
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

	size_t ret = m_typeRenderer.DetectAndRenderPointer(address);

	if (ret > 0 && relativeOffset > 0) {
		ImGui::SameLine(0, 2);
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, .5f, .5f, 1));
		if (displayOffset < 0) {
			ImGui::Text("-%X", abs(displayOffset));
		} else {
			ImGui::Text("+%X", displayOffset);
		}
		ImGui::PopStyleColor();
	}

	return (uint16_t)ret;
}

const char* DataTab::GetTitlePrefix()
{
	return ICON_FA_DATABASE;
}

void DataTab::RenderMenu(float dt)
{
	MemoryTab::RenderMenu(dt);

	if (ImGui::BeginMenu("Data")) {
		if (ImGui::MenuItem("Reset base offset", nullptr, nullptr, m_baseOffset > 0)) {
			m_baseOffset = 0;
		}
		if (ImGui::MenuItem("Reset base size", nullptr, nullptr, m_baseSize > 0)) {
			m_baseSize = 0;
		}

		ImGui::EndMenu();
	}
}

void DataTab::Render(float dt)
{
	m_lineDetails.ensure_memory(m_itemsPerPage + 1);
	while (m_lineDetails.len() < m_itemsPerPage + 1) {
		m_lineDetails.add();
	}

	size_t base = m_region.m_start;
	size_t size = m_region.m_end - m_region.m_start;

	uint16_t currentMemberSize = 0;

	auto handle = m_inspector->m_processHandle;

	if (ImGui::BeginTable("Code", 5, ImGuiTableFlags_Resizable)) {
		ImGui::TableSetupColumn("Address", ImGuiTableColumnFlags_WidthFixed);
		ImGui::TableSetupColumn("Offset", ImGuiTableColumnFlags_WidthFixed, 100);
		ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthFixed);
		ImGui::TableSetupColumn("Bytes", ImGuiTableColumnFlags_WidthFixed);
		ImGui::TableSetupColumn("Comments", ImGuiTableColumnFlags_WidthStretch);
		ImGui::TableHeadersRow();

		for (int i = 0; i < m_itemsPerPage + 1; i++) {
			uintptr_t offset = (uintptr_t)m_topOffset + i * sizeof(uintptr_t);
			uintptr_t address = base + offset;

			if (!m_inspector->m_processHandle->IsReadableMemory(address)) {
				break;
			}

			ImGui::TableNextRow(ImGuiTableRowFlags_None, m_itemHeight);

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

			ImGui::TableSetColumnIndex(0);

			RenderAddress(address);
			ImGui::SameLine();

			ImGui::TableSetColumnIndex(1);

			ImGui::PushFont(Resources::FontMono);
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

			ImGui::SameLine();

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

			uintptr_t value = 0;
			if (handle->ReadMemory(address, &value, sizeof(value)) == sizeof(value)) {
				ImGui::TableSetColumnIndex(2);
				ImGui::PushFont(Resources::FontMono);

				if (value == 0) {
					ImGui::TextDisabled(POINTER_FORMAT, value);
				} else {
					ImGui::Text(POINTER_FORMAT, value);
				}

				ImGui::TableSetColumnIndex(3);

				for (int i = 0; i < sizeof(uintptr_t); i++) {
					int shift = i * 8;
					uint8_t byteValue = (value & ((uintptr_t)0xFF << shift)) >> shift;

					if (i > 0) {
						ImGui::SameLine(0, 0);
					}

					if (byteValue >= 32 && byteValue <= 126) {
						ImGui::Text("%c", byteValue);
					} else {
						ImGui::TextDisabled(".");
					}
				}

				ImGui::PopFont();
			}

			ImGui::TableSetColumnIndex(4);

			s2::string symbolName;
			if (handle->GetSymbolName(address, symbolName)) {
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, .5f, .5f, 1));
				ImGui::TextUnformatted(symbolName);
				ImGui::PopStyleColor();
				ImGui::SameLine();
			}

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

		ImGui::EndTable();
	}
}
