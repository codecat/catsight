#pragma once

#include <Common.h>
#include <System/ProcessHandle.h>
#include <System/ProcessInfo.h>

#include <mutex>

class LinuxProcessHandle : public ProcessHandle
{
private:
	int m_pid;
	FILE* m_fhMemory;
	std::mutex m_readLock;

public:
	LinuxProcessHandle(const ProcessInfo& info);
	virtual ~LinuxProcessHandle();

	virtual bool IsOpen() override;

	virtual size_t ReadMemory(uintptr_t p, void* buffer, size_t size) override;
	virtual bool IsReadableMemory(uintptr_t p) override;
	virtual bool IsExecutableMemory(uintptr_t p) override;

	virtual s2::list<ProcessMemoryRegion> GetMemoryRegions() override;
};
