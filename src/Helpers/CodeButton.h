#pragma once

#include <Common.h>

class Inspector;

namespace Helpers
{
	void CodeButton(Inspector* inspector, uintptr_t p, const char* label = "Code", const char* newTabName = "Code");
}
