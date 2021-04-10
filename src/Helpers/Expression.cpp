#include <Common.h>
#include <Helpers/Expression.h>
#include <Inspector.h>
#include <Chrono.h>

uintptr_t Helpers::EvaluateExpression(Inspector* inspector, const char* sz)
{
	//TODO: Actually evaluate expressions. For now, only reads hex values, labels, and symbol names!
	// Could use Lua for this?

	uintptr_t ret = 0;

	if (inspector->m_labels.GetAddress(sz, ret)) {
		return ret;
	}

	if (inspector->m_processHandle->GetSymbolAddress(sz, ret)) {
		return ret;
	}

	sscanf(sz, "%llx", &ret);
	return ret;
}
