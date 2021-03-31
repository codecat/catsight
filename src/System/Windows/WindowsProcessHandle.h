#pragma once

#include <Common.h>
#include <System/ProcessHandle.h>
#include <System/ProcessInfo.h>

class WindowsProcessHandle : public ProcessHandle
{
private:
	void* m_proc = nullptr;

public:
	WindowsProcessHandle(const ProcessInfo& info);
	virtual ~WindowsProcessHandle();

	virtual bool IsOpen() override;

	virtual size_t ReadMemory(uintptr_t p, void* buffer, size_t size) override;
	virtual bool IsReadableMemory(uintptr_t p) override;
	virtual bool IsExecutableMemory(uintptr_t p) override;

	virtual s2::list<ProcessMemoryRegion> GetMemoryRegions() override;
};
