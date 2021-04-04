#pragma once

#include <Common.h>
#include <System/ProcessInfo.h>
#include <System/ProcessHandle.h>
#include <Tasks/Tasks.h>
#include <Tab.h>
#include <Script/Engine.h>

#include <mutex>

class Inspector
{
private:
	s2::string m_title;

public:
	bool m_isOpen = true;

	Tasks m_tasks;

	ProcessInfo m_processInfo;
	ProcessHandle* m_processHandle = nullptr;
	s2::list<ProcessMemoryRegion> m_processRegions;
	std::mutex m_processRegionsMutex; //TODO: Can we get rid of this **here**? Move it somewhere else? ProcessMemoryRegionCollection?

	s2::list<Tab*> m_tabs;

	Script::Engine m_script;

public:
	Inspector(const ProcessInfo& info);
	~Inspector();

	const ProcessInfo& GetProcessInfo();

	bool GetMemoryRegion(uintptr_t p, ProcessMemoryRegion& region);
	void UpdateMemoryRegions();

	void Render(float dt);
	void Update(float dt);

	template<typename T>
	T* GetFirstTab()
	{
		for (auto tab : m_tabs) {
			if (dynamic_cast<T*>(tab) != nullptr) {
				return (T*)tab;
			}
		}
		return nullptr;
	}

private:
	void RenderMenu(float dt);
	void RenderTabs(float dt);
};
