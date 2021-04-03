#pragma once

#include <Common.h>

namespace Script::Types
{
	class Pointer
	{
	private:
		uintptr_t m_p;

	public:
		Pointer();
		Pointer(uintptr_t p);
	};
}
