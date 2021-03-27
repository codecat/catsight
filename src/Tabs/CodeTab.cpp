#include <Common.h>
#include <Tabs/CodeTab.h>
#include <Inspector.h>
#include <Resources.h>

#include <hello_imgui.h>

CodeTab::CodeTab(Inspector* inspector, const s2::string& name, uintptr_t p)
	: MemoryTab(inspector, name, p)
{
	//TODO: Different parameters for 32 bit
	ZydisDecoderInit(&m_decoder, ZYDIS_MACHINE_MODE_LONG_64, ZYDIS_ADDRESS_WIDTH_64);
	ZydisFormatterInit(&m_formatter, ZYDIS_FORMATTER_STYLE_INTEL);

	m_showScrollBar = false;
}

CodeTab::~CodeTab()
{
}

s2::string CodeTab::GetLabel()
{
	return s2::strprintf(ICON_FA_CODE " %s (" POINTER_FORMAT ")###Code", MemoryTab::GetLabel().c_str(), m_region.m_start + m_baseOffset);
}

void CodeTab::Render()
{
	size_t base = m_region.m_start;
	size_t size = m_region.m_end - m_region.m_start;

	int bytesOffset = 0;
	for (int i = 0; i < m_itemsPerPage + 1; i++) {
		uintptr_t offset = m_topOffset + bytesOffset;
		uintptr_t address = base + offset;

		intptr_t displayOffset = (intptr_t)offset - (intptr_t)m_baseOffset;

		ImGui::PushID((void*)address);
		ImGui::PushFont(Resources::FontMono);

		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(.5f, .5f, .5f, 1));
		ImGui::Text(POINTER_FORMAT, address);
		ImGui::PopStyleColor();
		ImGui::SameLine();

		float column = 130.0f;

		if (ImGui::Button("$")) {
			m_baseOffset = offset;
		}

		column += 30;
		ImGui::SameLine(column);

		ImGui::PushStyleColor(ImGuiCol_Text, ImColor::HSV(0.0f, 0.65f, 1.0f).Value);
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

		uint8_t buffer[MAX_INSTRUCTION_SIZE];
		size_t bufferSize = m_inspector->m_processHandle->ReadMemory(address, buffer, sizeof(buffer));

		ZydisDecodedInstruction instr;
		if (ZYAN_SUCCESS(ZydisDecoderDecodeBuffer(&m_decoder, buffer, bufferSize, &instr))) {
			char instructionText[256];
			ZydisFormatterFormatInstruction(&m_formatter, &instr, instructionText, sizeof(instructionText), address);

			if (instr.opcode == 0x90 || instr.opcode == 0xCC) {
				ImGui::TextDisabled("%s", instructionText);
			} else {
				ImGui::Text("%s", instructionText);
			}
			bytesOffset += instr.length;
		} else {
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, .5f, .5f, 1));
			ImGui::TextUnformatted("??");
			ImGui::PopStyleColor();
			bytesOffset++;
		}

		ImGui::PopID();
	}
}

intptr_t CodeTab::GetScrollAmount(int wheel)
{
	const int numInstructionsPerRotation = 4;
	int numInstructions = numInstructionsPerRotation * abs(wheel);

	uintptr_t address = m_region.m_start + m_topOffset;

	if (wheel < 0) {
		// Scroll down
		int bytesOffset = 0;
		for (int i = 0; i < numInstructions; i++) {
			uint8_t buffer[MAX_INSTRUCTION_SIZE];
			size_t bufferSize = m_inspector->m_processHandle->ReadMemory(address + bytesOffset, buffer, sizeof(buffer));

			ZydisDecodedInstruction instr;
			if (ZYAN_SUCCESS(ZydisDecoderDecodeBuffer(&m_decoder, buffer, bufferSize, &instr))) {
				bytesOffset += instr.length;
			} else {
				bytesOffset++;
			}
		}
		return bytesOffset;

	} else {
		// Scroll up
		size_t bufferSize = (numInstructions + 3) * MAX_INSTRUCTION_SIZE;
		uint8_t* buffer = (uint8_t*)alloca(bufferSize + 1 + MAX_INSTRUCTION_SIZE);

		uintptr_t start = address - bufferSize;
		m_inspector->m_processHandle->ReadMemory(start, buffer, bufferSize + 1 + MAX_INSTRUCTION_SIZE);

		uintptr_t bufferOffset = DisassembleBack(buffer, bufferSize, bufferSize, numInstructions);
		uintptr_t newOffset = start + bufferOffset;

		return newOffset - address;
	}
}

uintptr_t CodeTab::DisassembleBack(const uint8_t* data, size_t size, uintptr_t ip, int n)
{
	// Function borrowed from x64dbg (QBeaEngine::DisassembleBack) which borrowed it from ollydbg

	if (data == nullptr) {
		return 0;
	}

	if (n < 0) {
		n = 0;
	} else if (n > 127) {
		n = 127;
	}

	if (ip >= size) {
		ip = size - 1;
	}

	if (n == 0 || ip < (uintptr_t)n) {
		return ip;
	}

	uintptr_t back = (n + 3) * MAX_INSTRUCTION_SIZE;
	if (ip < back) {
		back = ip;
	}

	uintptr_t addr = ip - back;
	const uint8_t* pdata = data + addr;

	uintptr_t abuf[128];
	int i = 0;
	while (addr < ip) {
		abuf[i % 128] = addr;

		uintptr_t length = 2;
		ZydisDecodedInstruction instr;
		if (ZYAN_SUCCESS(ZydisDecoderDecodeBuffer(&m_decoder, pdata, size, &instr))) {
			length = instr.length;
		}

		pdata += length;
		addr += length;
		back -= length;
		size -= length;

		i++;
	}

	if (i < n) {
		return abuf[0];
	}
	return abuf[(i - n + 128) % 128];
}
