#include <Common.h>
#include <Helpers/Expression.h>
#include <Inspector.h>
#include <Chrono.h>

uintptr_t Helpers::EvaluateExpression(Inspector* inspector, const char* sz)
{
	//TODO: Actually evaluate expressions. For now, only reads hex values and symbol names!

	uintptr_t ret = 0;

	auto tmStart = Chrono::Now();
	if (inspector->m_processHandle->GetSymbolAddress(sz, ret)) {
		return ret;
	}

	sscanf(sz, "%llx", &ret);
	return ret;
}
