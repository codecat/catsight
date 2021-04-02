#include <Common.h>
#include <Disassembler.h>

Disassembler::Disassembler()
{
	//TODO: Different parameters for 32 bit
	ZydisDecoderInit(&m_decoder, ZYDIS_MACHINE_MODE_LONG_64, ZYDIS_ADDRESS_WIDTH_64);
	ZydisFormatterInit(&m_formatter, ZYDIS_FORMATTER_STYLE_INTEL);
}

Disassembler::~Disassembler()
{
}

bool Disassembler::Decode(ZydisDecodedInstruction& instr, ProcessHandle* handle, uintptr_t p)
{
	uint8_t buffer[MAX_INSTRUCTION_SIZE];

	size_t size = handle->ReadMemory(p, buffer, sizeof(buffer));
	if (size == 0) {
		return false;
	}

	return Decode(instr, buffer, size);
}

bool Disassembler::Decode(ZydisDecodedInstruction& instr, const uint8_t* buffer, size_t bufferSize)
{
	return ZYAN_SUCCESS(ZydisDecoderDecodeBuffer(&m_decoder, buffer, bufferSize, &instr));
}

s2::string Disassembler::Format(const ZydisDecodedInstruction& instr, uintptr_t address)
{
	char ret[256] = "??";
	ZydisFormatterFormatInstruction(&m_formatter, &instr, ret, sizeof(ret), address);
	return ret;
}

InstructionByteGroups Disassembler::GetByteGroups(const ZydisDecodedInstruction& instr)
{
	InstructionByteGroups ret;
	ret.m_sizePrefix = instr.raw.prefix_count;
	ret.m_sizeGroup1 = instr.raw.disp.size / 8;
	ret.m_sizeGroup2 = instr.raw.imm[0].size / 8;
	ret.m_sizeGroup3 = instr.raw.imm[1].size / 8;
	ret.m_sizeOpcode = instr.length - ret.m_sizePrefix - ret.m_sizeGroup1 - ret.m_sizeGroup2 - ret.m_sizeGroup3;
	return ret;
}
