#pragma once

#include <Common.h>

#include <System/ProcessHandle.h>
#include <Tasks/Task.h>

namespace Patterns
{
	typedef s2::func<void(uintptr_t)> Callback;

	void Find(ProcessHandle* handle, const char* pattern, const Callback& callback, Task* task, const ProcessMemoryRegion& region);
	void Find(ProcessHandle* handle, const char* pattern, const Callback& callback, Task* task = nullptr);

	s2::string Generate(ProcessHandle* handle, uintptr_t address, const ProcessMemoryRegion& region);
}
