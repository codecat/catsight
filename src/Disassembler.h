#pragma once

#include <Common.h>
#include <System/ProcessHandle.h>

#include <Zydis/Zydis.h>

struct InstructionByteGroups
{
	size_t m_sizePrefix;
	size_t m_sizeGroup1;
	size_t m_sizeGroup2;
	size_t m_sizeGroup3;
	size_t m_sizeOpcode;
};

class Disassembler
{
private:
	ZydisDecoder m_decoder;
	ZydisFormatter m_formatter;

public:
	Disassembler();
	~Disassembler();

	bool Decode(ZydisDecodedInstruction& instr, ProcessHandle* handle, uintptr_t p);
	bool Decode(ZydisDecodedInstruction& instr, const uint8_t* buffer, size_t bufferSize);

	s2::string Format(const ZydisDecodedInstruction& instr, uintptr_t address = 0);

	InstructionByteGroups GetByteGroups(const ZydisDecodedInstruction& instr);
};
