#pragma once

#include <Common.h>

class Inspector;

class TypeRenderer
{
public:
	bool m_resolveFloats = false;
	bool m_resolvePointersIfAligned = true;
	int m_resolvePointerMaxDepth = 4;

private:
	Inspector* m_inspector;

public:
	TypeRenderer(Inspector* inspector);
	~TypeRenderer();

	bool RenderMenu();

	size_t DetectAndRenderPointer(uintptr_t p, int depth = 0);
	size_t DetectAndRenderType(uintptr_t value, size_t limitedSize = sizeof(uintptr_t), int depth = 0);
};
