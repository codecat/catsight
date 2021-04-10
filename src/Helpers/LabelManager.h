#pragma once

#include <Common.h>
#include <Core/Hashtable.h>
#include <Core/Sortdict.h>
#include <System/ProcessMemoryRegion.h>

class Inspector;

class LabelManager
{
private:
	struct Module
	{
		s2::string m_path;

		hashtable<uintptr_t> m_addresses;
		sortdict<uintptr_t, s2::string> m_labels;
	};

private:
	Inspector* m_inspector;
	hashtable<Module*> m_modules;

public:
	LabelManager(Inspector* inspector);
	~LabelManager();

	void Set(uintptr_t p, const char* label);
	void Set(uintptr_t p, const char* label, const ProcessMemoryRegion& region);
	void Remove(uintptr_t p);
	void Remove(uintptr_t p, const ProcessMemoryRegion& region);

	bool GetLabel(uintptr_t p, s2::string& label);
	bool GetLabel(uintptr_t p, s2::string& label, const ProcessMemoryRegion& region);
	bool GetAddress(const char* label, uintptr_t& p);
	bool GetAddress(const char* label, uintptr_t& p, const ProcessMemoryRegion& region);

private:
	Module* GetModule(const ProcessMemoryRegion& region);
};
