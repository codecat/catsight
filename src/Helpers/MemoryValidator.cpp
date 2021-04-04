#include <Common.h>
#include <Helpers/MemoryValidator.h>

bool MemoryValidator::String(ProcessHandle* handle, uintptr_t p)
{
	// It might be a string if:
	// - The relative offset is 0
	// - The pointer is not 0
	// - The pointer is valid and can be read
	// - There are at least 5 printable characters

	if (p == 0) {
		return false;
	}

	if (!handle->IsReadableMemory(p)) {
		return false;
	}

	uint8_t buffer[5];
	handle->ReadMemory(p, buffer, sizeof(buffer));

	for (size_t i = 0; i < sizeof(buffer); i++) {
		uint8_t c = buffer[i];
		if (c < 0x20 || c > 0x7E) {
			break;
		}
		if (i == 4) {
			return true;
		}
	}

	return false;
}

bool MemoryValidator::String(ProcessHandle* handle, uintptr_t p, s2::string& str)
{
	if (!String(handle, p)) {
		return false;
	}

	handle->ReadCString(p, str);
	return true;
}

bool MemoryValidator::Float(float f)
{
	// The 32 bit integer must not be 0
	uint32_t u32 = *(uint32_t*)&f;
	if (u32 == 0) {
		return false;
	}

	// The float must not be NaN or infinity
	if (std::isnan(f) || std::isinf(f)) {
		return false;
	}

	// The float must be in an acceptable range
	if (fabsf(f) < 0.0001f || fabsf(f) > 100000.0f) {
		return false;
	}

	return true;
}
