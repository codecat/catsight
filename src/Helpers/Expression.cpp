#include <Common.h>
#include <Helpers/Expression.h>

uintptr_t Helpers::EvaluateExpression(const char* sz)
{
	//TODO: Actually evaluate expressions. For now, only reads hex values!
	uintptr_t ret = 0;
	sscanf(sz, "%llx", &ret);
	return ret;
}
