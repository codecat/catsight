#pragma once

#include <Common.h>
#include <Tabs/MemoryTab.h>
#include <Disassembler.h>

class CodeTab : public MemoryTab
{
private:
	uintptr_t m_baseOffset = 0;
	bool m_hasBaseOffset = false;

	Disassembler m_disasm;

	struct LineDetails
	{
		bool m_memoryExecutable = false;
		bool m_pointsToExecutable = false;
		intptr_t m_jumpsLines = 0;
		int m_depth = 0;
	};
	s2::list<LineDetails> m_lineDetails;

	bool m_ui_findConstantPopupShow = false;
	s2::string m_ui_findConstantValueString;

	bool m_ui_findPatternPopupShow = false;
	s2::string m_ui_findPatternValueString;

public:
	CodeTab(Inspector* inspector, const s2::string& id, uintptr_t p);
	virtual ~CodeTab();

	virtual const char* GetTitlePrefix() override;

	virtual void RenderMenu(float dt) override;
	virtual void Render(float dt) override;

protected:
	virtual intptr_t GetScrollAmount(int wheel) override;

private:
	uintptr_t DisassembleBack(const uint8_t* data, size_t size, uintptr_t ip, int n);

	static uintptr_t GetOperandValue(ZydisDecodedInstruction& instr, int i, uintptr_t address);
};
