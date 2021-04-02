#pragma once

#include <Common.h>

class Inspector;

namespace Helpers
{
	typedef s2::func<void(uintptr_t)> PointerMenuCallback;

	void PointerText(Inspector* inspector, uintptr_t p, const PointerMenuCallback& menuCallback = nullptr);
}
