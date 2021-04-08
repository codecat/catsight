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

	struct SectionInfo
	{
		uintptr_t m_offset;
		uintptr_t m_size;
		s2::string m_name;
	};

	struct ModuleInfo
	{
		s2::string m_path;
		uintptr_t m_start = 0;
		uintptr_t m_entryPoint = 0;
		s2::list<SectionInfo> m_sections;
	};

	s2::list<ModuleInfo> m_modules;

public:
	LinuxProcessHandle(const ProcessInfo& info);
	virtual ~LinuxProcessHandle();

	virtual bool IsOpen() override;

	virtual size_t ReadMemory(uintptr_t p, void* buffer, size_t size) override;
	virtual bool IsReadableMemory(uintptr_t p) override;
	virtual bool IsExecutableMemory(uintptr_t p) override;

	virtual bool GetSymbolName(uintptr_t p, s2::string& name) override;
	virtual bool GetSymbolAddress(const char* name, uintptr_t& p) override;

	virtual s2::list<ProcessMemoryRegion> GetMemoryRegions() override;
};
