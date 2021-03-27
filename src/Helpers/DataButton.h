#pragma once

#include <Common.h>

class Inspector;

namespace Helpers
{
	void DataButton(Inspector* inspector, uintptr_t p, const char* label = "Data", const char* newTabName = "Data");
}
