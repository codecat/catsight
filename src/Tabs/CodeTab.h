#pragma once

#include <Common.h>
#include <Tabs/MemoryTab.h>

#include <Zydis/Zydis.h>

class CodeTab : public MemoryTab
{
private:
	uintptr_t m_baseOffset = 0;
	bool m_hasBaseOffset = false;

	ZydisDecoder m_decoder;
	ZydisFormatter m_formatter;

	struct LineDetails
	{
		bool m_memoryExecutable = false;
		bool m_pointsToExecutable = false;
		intptr_t m_jumpsLines = 0;
		int m_depth = 0;
	};
	s2::list<LineDetails> m_lineDetails;

public:
	CodeTab(Inspector* inspector, const s2::string& name, uintptr_t p);
	virtual ~CodeTab();

	virtual s2::string GetLabel() override;

	virtual void RenderMenu(float dt) override;
	virtual void Render(float dt) override;

protected:
	virtual intptr_t GetScrollAmount(int wheel) override;

private:
	uintptr_t DisassembleBack(const uint8_t* data, size_t size, uintptr_t ip, int n);

	static uintptr_t GetOperandValue(ZydisDecodedInstruction& instr, int i, uintptr_t address);
};
