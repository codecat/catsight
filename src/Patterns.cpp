#include <Common.h>
#include <Patterns.h>
#include <Disassembler.h>

struct PatternByte
{
	uint8_t byte;
	uint8_t mask;
};

void Patterns::Find(ProcessHandle* handle, const char* pattern, const Callback& callback, Task* task, const ProcessMemoryRegion& region)
{
	PatternByte* compiled = nullptr;

	int numBytes = ((int)strlen(pattern) + 1) / 3;
	if (numBytes == 0) {
		return;
	}

	compiled = (PatternByte*)alloca(numBytes * sizeof(PatternByte));

	int byteIndex = 0;

	for (const char* p = pattern; *p != '\0'; p++) {
		if (*p == ' ') { continue; }
		if (*(p + 1) == '\0') { break; }
		PatternByte &byte = compiled[byteIndex];

		if (*p == '?') {
			byte.mask = 1;

			p++;
			byteIndex++;
			continue;
		}

		sscanf(p, "%hhx", &byte.byte);
		byte.mask = 0;

		byteIndex++;
		p++;
	}

	if (byteIndex == 0) {
		return;
	}

	size_t startAddress = 0;
	size_t cur = 0;

	uintptr_t start = region.m_start;
	uintptr_t end = region.m_end;
	size_t size = region.Size();

	//TODO: Optimization: read larger buffers from process instead of byte-per-byte to avoid the overhead of remote memory access and virtual function calls

	for (uintptr_t p = start; p < end; p++) {
		auto& byte = compiled[cur];

		if (byte.mask == 1 || handle->Read<uint8_t>(p) == byte.byte) {
			if (cur == 0) {
				startAddress = p;
			}
			if (++cur == numBytes) {
				callback(startAddress);
				cur = 0;
			}
		} else if (cur > 0) {
			cur = 0;
			p = startAddress;
		}

		if (task != nullptr) {
			task->m_progress = (float)((p - start) / (double)size);
		}
	}
}

void Patterns::Find(ProcessHandle* handle, const char* pattern, const Callback& callback, Task* task)
{
	auto regions = handle->GetMemoryRegions();
	for (auto& region : regions) {
		Find(handle, pattern, callback, task, region);
	}
}

s2::string Patterns::Generate(ProcessHandle* handle, uintptr_t address, const ProcessMemoryRegion& region)
{
	s2::string ret;

	Disassembler disasm;

	uintptr_t p = address;
	int finds = 0;

	int byteCount = 0;

	do {
		do {
			// Read memory
			uint8_t buffer[MAX_INSTRUCTION_SIZE];
			size_t bufferSize = handle->ReadMemory(p, buffer, sizeof(buffer));
			if (bufferSize == 0) {
				break;
			}

			// Decode instruction
			ZydisDecodedInstruction instr;
			if (!disasm.Decode(instr, buffer, bufferSize)) {
				printf("Invalid assembly detected at " POINTER_FORMAT "!\n", p);
				break;
			}

			// Increment instruction pointer for next iteration
			p += instr.length;

			// Count the bytes
			byteCount += instr.length;

			// Get instruction byte groups and "real" length (excluding offset operands)
			auto groups = disasm.GetByteGroups(instr);
			size_t realLength = groups.m_sizePrefix + groups.m_sizeOpcode;

			// Write pattern to string
			for (size_t i = 0; i < instr.length; i++) {
				if (i < realLength) {
					ret.appendf("%02X", buffer[i]);
				} else {
					ret += "??";
				}
				ret.append(' ');
			}
		} while (byteCount < 5);

		// Stop if we reached too many bytes
		if (byteCount > 150) {
			printf("Stopping pattern generation prematurely because %d bytes were reached!\n", byteCount);
			break;
		}

		// Count the amount of results
		finds = 0;
		Find(handle, ret, [&finds](uintptr_t) { finds++; }, nullptr, region);
	} while (finds > 1);

	assert(finds == 1);

	return ret.trim(" ?");
}
