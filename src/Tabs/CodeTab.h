#pragma once

#include <Common.h>
#include <Tabs/MemoryTab.h>

#include <Zydis/Zydis.h>

class CodeTab : public MemoryTab
{
private:
	uintptr_t m_baseOffset = 0;

	ZydisDecoder m_decoder;
	ZydisFormatter m_formatter;

	struct LineDetails
	{
		bool m_memoryExecutable = false;
	};
	s2::list<LineDetails> m_lineDetails;

public:
	CodeTab(Inspector* inspector, const s2::string& name, uintptr_t p);
	virtual ~CodeTab();

	virtual s2::string GetLabel() override;

	virtual void Render() override;

protected:
	virtual intptr_t GetScrollAmount(int wheel) override;

private:
	uintptr_t DisassembleBack(const uint8_t* data, size_t size, uintptr_t ip, int n);
};
