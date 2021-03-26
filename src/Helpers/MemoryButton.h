#pragma once

#include <Common.h>

class Inspector;

namespace Helpers
{
	void MemoryButton(Inspector* inspector, uintptr_t p, const char* label, const char* newTabName = "Memory");
}
