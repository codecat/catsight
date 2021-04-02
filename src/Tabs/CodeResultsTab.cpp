#include <Common.h>
#include <Tabs/CodeResultsTab.h>
#include <Inspector.h>
#include <Helpers/CodeButton.h>

#include <hello_imgui.h>

CodeResultsTab::CodeResultsTab(Inspector* inspector, const s2::string& name)
	: ResultsTab(inspector, name)
{
	//TODO: Different parameters for 32 bit
	ZydisDecoderInit(&m_decoder, ZYDIS_MACHINE_MODE_LONG_64, ZYDIS_ADDRESS_WIDTH_64);
	ZydisFormatterInit(&m_formatter, ZYDIS_FORMATTER_STYLE_INTEL);
}

CodeResultsTab::~CodeResultsTab()
{
}

void CodeResultsTab::RenderResult(const Result& result)
{
	Helpers::CodeButton(m_inspector, result.m_address);

	ImGui::SameLine();

	ResultsTab::RenderResult(result);

	ImGui::SameLine();

	uint8_t buffer[MAX_INSTRUCTION_SIZE];
	m_inspector->m_processHandle->ReadMemory(result.m_address, buffer, sizeof(buffer));

	ZydisDecodedInstruction instr;
	if (ZYAN_SUCCESS(ZydisDecoderDecodeBuffer(&m_decoder, buffer, sizeof(buffer), &instr))) {
		char instructionText[256] = "??";
		ZydisFormatterFormatInstruction(&m_formatter, &instr, instructionText, sizeof(instructionText), result.m_address);
		ImGui::TextUnformatted(instructionText);
	}
}
