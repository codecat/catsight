#pragma once

#include <Common.h>
#include <System/ProcessMemoryRegion.h>

class ProcessHandle
{
public:
	virtual ~ProcessHandle();

	virtual bool IsOpen() = 0;

	virtual size_t ReadMemory(uintptr_t p, void* buffer, size_t size) = 0;
	virtual bool IsReadableMemory(uintptr_t p) = 0;
	virtual bool IsExecutableMemory(uintptr_t p) = 0;

	virtual bool GetSymbolName(uintptr_t p, s2::string& name) = 0;
	virtual bool GetSymbolAddress(const char* name, uintptr_t& p) = 0;

	virtual s2::list<ProcessMemoryRegion> GetMemoryRegions() = 0;

public:
	template<typename T>
	T Read(uintptr_t p)
	{
		T ret;
		ReadMemory(p, &ret, sizeof(T));
		return ret;
	}

	void ReadCString(uintptr_t p, s2::string& str, int limit = -1);
};
