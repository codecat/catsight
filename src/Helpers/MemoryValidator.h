#pragma once

#include <Common.h>
#include <System/ProcessHandle.h>

namespace MemoryValidator
{
	bool String(ProcessHandle* handle, uintptr_t p);
	bool String(ProcessHandle* handle, uintptr_t p, s2::string& str);
	bool Float(float f);
}
