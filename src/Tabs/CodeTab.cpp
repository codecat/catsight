#include <Common.h>
#include <Tabs/CodeTab.h>
#include <Inspector.h>
#include <Resources.h>
#include <Chrono.h>
#include <Patterns.h>

#include <Helpers/MemoryValidator.h>
#include <Helpers/CodeButton.h>
#include <Helpers/DataButton.h>
#include <Helpers/PointerText.h>
#include <Helpers/ImGuiString.h>
#include <Helpers/Expression.h>

#include <Tabs/StringsTab.h>
#include <Tabs/CodeResultsTab.h>

#include <hello_imgui.h>

#include <chrono>

CodeTab::CodeTab(Inspector* inspector, const s2::string& id, uintptr_t p)
	: MemoryTab(inspector, id, p)
{
	//TODO: Different parameters for 32 bit
	ZydisDecoderInit(&m_decoder, ZYDIS_MACHINE_MODE_LONG_64, ZYDIS_ADDRESS_WIDTH_64);
	ZydisFormatterInit(&m_formatter, ZYDIS_FORMATTER_STYLE_INTEL);

	m_showScrollBar = false;
}

CodeTab::~CodeTab()
{
}

const char* CodeTab::GetTitlePrefix()
{
	return ICON_FA_CODE;
}

void CodeTab::RenderMenu(float dt)
{
	MemoryTab::RenderMenu(dt);

	if (ImGui::BeginMenu("Find")) {
		if (ImGui::MenuItem("All referenced strings")) {
			auto handle = m_inspector->m_processHandle;
			auto region = m_region;
			auto decoder = m_decoder;

			auto stringsTab = new StringsTab(m_inspector, "Strings");
			m_inspector->m_tabs.add(stringsTab);

			stringsTab->m_task = m_inspector->m_tasks.Run([handle, region, decoder, stringsTab](Task* task) {
				uintptr_t offset = 0;
				uint8_t buffer[MAX_INSTRUCTION_SIZE];

				auto tmStart = std::chrono::high_resolution_clock::now();

				while (offset < region.Size()) {
					uintptr_t address = region.m_start + offset;

					size_t bytesRead = handle->ReadMemory(address, buffer, sizeof(buffer));
					if (bytesRead == 0) {
						break;
					}

					ZydisDecodedInstruction instr;
					if (!ZYAN_SUCCESS(ZydisDecoderDecodeBuffer(&decoder, buffer, sizeof(buffer), &instr))) {
						offset++;
						continue;
					}

					// Go through all instruction operands and find valid pointers
					for (uint8_t j = 0; j < instr.operand_count; j++) {
						uintptr_t operandValue = GetOperandValue(instr, j, address);

						if (handle->IsReadableMemory(operandValue) && MemoryValidator::String(handle, operandValue)) {
							auto& newResult = stringsTab->m_results.add();
							newResult.m_address = address;
							newResult.m_value = operandValue;
						}
					}

					offset += instr.length;
					task->m_progress = (float)(offset / (double)region.Size());
				}

				auto tmDuration = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - tmStart).count();
				printf("Done searching referenced strings in %.02f milliseconds\n", tmDuration / 1000.0f);
			})->Then([stringsTab](Task*) {
				stringsTab->TaskFinished();
			});
		}

		if (ImGui::MenuItem("Constant...")) {
			m_ui_findConstantPopupShow = true;
		}

		if (ImGui::MenuItem("Pattern...", "Ctrl+B")) {
			m_ui_findPatternPopupShow = true;
		}

		ImGui::EndMenu();
	}
}

void CodeTab::Render(float dt)
{
	// Find constant
	if (m_ui_findConstantPopupShow) {
		ImGui::OpenPopup("FindConstantPopup");
		m_ui_findConstantPopupShow = false;
	}
	if (ImGui::BeginPopup("FindConstantPopup")) {
		if (ImGui::IsWindowAppearing()) {
			ImGui::SetKeyboardFocusHere();
		}
		if (Helpers::InputText("Value", &m_ui_findConstantValueString, ImGuiInputTextFlags_EnterReturnsTrue)) {
			uintptr_t value = Helpers::EvaluateExpression(m_ui_findConstantValueString);

			auto handle = m_inspector->m_processHandle;
			auto region = m_region;
			auto decoder = m_decoder;

			auto newTab = new CodeResultsTab(m_inspector, "Constants");
			m_inspector->m_tabs.add(newTab);

			newTab->m_task = m_inspector->m_tasks.Run([value, handle, region, decoder, newTab](Task* task) {
				uintptr_t offset = 0;
				uint8_t buffer[MAX_INSTRUCTION_SIZE];

				auto tmStart = std::chrono::high_resolution_clock::now();

				while (offset < region.Size()) {
					uintptr_t address = region.m_start + offset;

					size_t bytesRead = handle->ReadMemory(address, buffer, sizeof(buffer));
					if (bytesRead == 0) {
						break;
					}

					ZydisDecodedInstruction instr;
					if (!ZYAN_SUCCESS(ZydisDecoderDecodeBuffer(&decoder, buffer, sizeof(buffer), &instr))) {
						offset++;
						continue;
					}

					// Go through all instruction operands and find the constant we want
					for (uint8_t j = 0; j < instr.operand_count; j++) {
						if (GetOperandValue(instr, j, address) == value) {
							auto& newResult = newTab->m_results.add();
							newResult.m_address = address;
						}
					}

					offset += instr.length;
					task->m_progress = (float)(offset / (double)region.Size());
				}

				auto tmDuration = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - tmStart).count();
				printf("Done searching for constant in %.02f milliseconds\n", tmDuration / 1000.0f);
			})->Then([newTab](Task*) {
				newTab->TaskFinished();
			});

			m_ui_findConstantValueString = "";
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}

	// Find pattern (Ctrl+B)
	if (ImGui::GetIO().KeyCtrl && ImGui::IsKeyPressed('B')) {
		m_ui_findPatternPopupShow = true;
	}
	if (m_ui_findPatternPopupShow) {
		ImGui::OpenPopup("FindPatternPopup");
		m_ui_findPatternPopupShow = false;
	}
	if (ImGui::BeginPopup("FindPatternPopup")) {
		if (ImGui::IsWindowAppearing()) {
			ImGui::SetKeyboardFocusHere();
		}
		if (Helpers::InputText("Pattern", &m_ui_findPatternValueString, ImGuiInputTextFlags_EnterReturnsTrue)) {
			auto pattern = m_ui_findPatternValueString;
			auto handle = m_inspector->m_processHandle;
			auto region = m_region;

			auto newTab = new CodeResultsTab(m_inspector, "Pattern");
			m_inspector->m_tabs.add(newTab);

			newTab->m_task = m_inspector->m_tasks.Run([pattern, handle, region, newTab](Task* task) {
				Patterns::Find(handle, pattern, [newTab](uintptr_t p) {
					auto& newResult = newTab->m_results.add();
					newResult.m_address = p;
				}, task, region);
			})->Then([newTab](Task*) {
				newTab->TaskFinished();
			});

			m_ui_findPatternValueString = "";
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}

	m_lineDetails.ensure_memory(m_itemsPerPage + 1);
	while (m_lineDetails.len() < m_itemsPerPage + 1) {
		m_lineDetails.add();
	}

	auto handle = m_inspector->m_processHandle;

	size_t base = m_region.m_start;
	size_t size = m_region.m_end - m_region.m_start;

	int lineDepth = 0;

	int bytesOffset = 0;
	for (int i = 0; i < m_itemsPerPage + 1; i++) {
		uintptr_t offset = m_topOffset + bytesOffset;
		uintptr_t address = base + offset;

		intptr_t displayOffset = (intptr_t)offset - (intptr_t)m_baseOffset;

		assert((size_t)i < m_lineDetails.len());
		auto& line = m_lineDetails[i];

		ImGui::PushID((void*)address);

		Helpers::PointerText(m_inspector, address);
		ImGui::SameLine();

		float column = 130.0f;

		ImGui::PushFont(Resources::FontMono);
		if (ImGui::Button("$")) {
			if (offset == m_baseOffset) {
				m_hasBaseOffset = !m_hasBaseOffset;
			} else {
				m_hasBaseOffset = true;
			}
			m_baseOffset = offset;
		}

		column += 30;
		ImGui::SameLine(column);

		if (m_hasBaseOffset) {
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

			column += 70;
			ImGui::SameLine(column);
		}

		uint8_t buffer[MAX_INSTRUCTION_SIZE];
		size_t bufferSize = handle->ReadMemory(address, buffer, sizeof(buffer));

		// Decode instruction
		ZydisDecodedInstruction instr;
		bool valid = ZYAN_SUCCESS(ZydisDecoderDecodeBuffer(&m_decoder, buffer, bufferSize, &instr));
		size_t instrSize = valid ? instr.length : 1;
		bytesOffset += instrSize;

		// Set instruction color
		ImVec4 color = ImGui::GetStyleColorVec4(ImGuiCol_Text);
		if (instr.mnemonic == ZYDIS_MNEMONIC_NOP || instr.mnemonic == ZYDIS_MNEMONIC_INT3) {
			color = ImVec4(.5f, .5f, .5f, 1);
		} else if (instr.meta.category == ZYDIS_CATEGORY_CALL) {
			color = ImVec4(1, .5f, .5f, 1);
		} else if (instr.meta.category == ZYDIS_CATEGORY_RET) {
			color = ImVec4(1, 1, .5f, 1);
		} else if (instr.meta.branch_type != ZYDIS_BRANCH_TYPE_NONE) {
			color = ImVec4(.5f, 1, 1, 1);
		}

		ImGui::PushStyleColor(ImGuiCol_Text, color);

		// Show instruction bytes
		size_t sizePrefix = instr.raw.prefix_count;
		size_t sizeGroup1 = instr.raw.disp.size / 8;
		size_t sizeGroup2 = instr.raw.imm[0].size / 8;
		size_t sizeGroup3 = instr.raw.imm[1].size / 8;
		size_t sizeOpcode = instr.length - sizePrefix - sizeGroup1 - sizeGroup2 - sizeGroup3;

		s2::string strBytes;
		for (size_t j = 0; j < instrSize; j++) {
			if (j > 0) {
				if (j == sizePrefix) {
					strBytes.append(':');
				} else if (j == sizePrefix + sizeOpcode) {
					strBytes.append(' ');
				} else if (j == sizePrefix + sizeOpcode + sizeGroup1) {
					strBytes.append(' ');
				} else if (j == sizePrefix + sizeOpcode + sizeGroup1 + sizeGroup2) {
					strBytes.append(' ');
				} else if (j == sizePrefix + sizeOpcode + sizeGroup1 + sizeGroup2 + sizeGroup3) {
					strBytes.append(' ');
				}
			}

			strBytes.appendf("%02X", buffer[j]);
		}
		ImGui::Text("%s", strBytes.c_str());

		column += 200;
		ImGui::SameLine(column);

		// Remember position of instruction text column for branch line drawing later on
		auto instructionPos = ImGui::GetCursorScreenPos();
		instructionPos.x -= 5.5f;
		instructionPos.y += m_itemHeight / 2 - 0.5f;

		// Show formatted instruction text
		char instructionText[256] = "??";
		if (valid) {
			ZydisFormatterFormatInstruction(&m_formatter, &instr, instructionText, sizeof(instructionText), address);
		}

		ImGui::Text("%s", instructionText);
		ImGui::PopStyleColor();

		ImGui::PopFont();
		ImGui::SameLine();

		if (valid) {
			if (m_invalidated) {
				line.m_jumpsLines = 0;
			}

			// Go through all instruction operands and find stuff we want to display
			for (uint8_t j = 0; j < instr.operand_count; j++) {
				uintptr_t operandValue = GetOperandValue(instr, j, address);

				if (handle->IsReadableMemory(operandValue)) {
					if (m_invalidated) {
						if (instr.meta.branch_type != ZYDIS_BRANCH_TYPE_NONE && instr.meta.category != ZYDIS_CATEGORY_CALL) {
							intptr_t jumpOffsetBytes = operandValue - address;

							//TODO: Support backwards jumps
							if (jumpOffsetBytes > 0) {
								uint8_t buffer[MAX_INSTRUCTION_SIZE];

								line.m_jumpsLines = 0;
								line.m_depth = ++lineDepth;
								int ip = 0;

								while (ip < jumpOffsetBytes) {
									handle->ReadMemory(address + ip, buffer, sizeof(buffer));

									ZydisDecodedInstruction instr;
									if (!ZYAN_SUCCESS(ZydisDecoderDecodeBuffer(&m_decoder, buffer, sizeof(buffer), &instr))) {
										break;
									}

									ip += instr.length;
									line.m_jumpsLines++;
									if (line.m_jumpsLines > m_itemsPerPage) {
										break;
									}
								}
							}
						}
					}

					m_typeRenderer.DetectAndRenderType(operandValue);
				} else {
					m_typeRenderer.DetectAndRenderType(operandValue, instr.operands[j].size / 8);
				}
			}

			// Draw lines
			if (line.m_jumpsLines != 0) {
				auto draw = ImGui::GetWindowDrawList();
				float offsetX = 5 * line.m_depth;
				draw->PathLineTo(ImVec2(instructionPos.x, instructionPos.y));
				draw->PathLineTo(ImVec2(instructionPos.x - offsetX, instructionPos.y));
				draw->PathLineTo(ImVec2(instructionPos.x - offsetX, instructionPos.y + m_itemHeight * line.m_jumpsLines));
				draw->PathLineTo(ImVec2(instructionPos.x, instructionPos.y + m_itemHeight * line.m_jumpsLines));
				draw->PathStroke(ImGui::GetColorU32(color));
			}
		}

		ImGui::NewLine();
		ImGui::PopID();
	}
}

intptr_t CodeTab::GetScrollAmount(int wheel)
{
	const int numInstructionsPerRotation = 3;
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
		size_t rvaSize = (numInstructions + 3) * MAX_INSTRUCTION_SIZE;
		size_t bufferSize = rvaSize + 1 + MAX_INSTRUCTION_SIZE;
		uint8_t* buffer = (uint8_t*)alloca(bufferSize);

		uintptr_t start = address - rvaSize;
		m_inspector->m_processHandle->ReadMemory(start, buffer, bufferSize);

		uintptr_t bufferOffset = DisassembleBack(buffer, bufferSize, rvaSize, numInstructions);
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

uintptr_t CodeTab::GetOperandValue(ZydisDecodedInstruction& instr, int i, uintptr_t address)
{
	//TODO: Move this function somewhere else?

	uintptr_t ret = 0;

	auto& op = instr.operands[i];

	if (op.type == ZYDIS_OPERAND_TYPE_IMMEDIATE) {
		if (op.imm.is_relative) {
			ZydisCalcAbsoluteAddress(&instr, &op, address, &ret);
		} else {
			ret = op.imm.value.u;
		}
	} else if (op.type == ZYDIS_OPERAND_TYPE_MEMORY) {
		ZydisCalcAbsoluteAddress(&instr, &op, address, &ret);
	}

	return ret;
}
