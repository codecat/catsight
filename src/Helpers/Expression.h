#pragma once

#include <Common.h>

class Inspector;

namespace Helpers
{
	uintptr_t EvaluateExpression(Inspector* inspector, const char* sz);
}
