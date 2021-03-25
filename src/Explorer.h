#pragma once

#include <Common.h>
#include <Inspector.h>
#include <ProcessInfo.h>

#include <hello_imgui.h>

class Explorer
{
public:
	static Explorer* Instance;

private:
	HelloImGui::RunnerParams m_params;
	s2::list<ProcessInfo> m_processes;
	UserInfo m_currentUser;
	UserInfo m_effectiveUser;

public:
	s2::list<Inspector*> m_inspectors;

public:
	void Run();

	void SetStyle();
	void LoadFonts();

	Inspector* GetInspector(const ProcessInfo& info);
	Inspector* GetInspector(int pid);

	void RenderMenu();
	void Render();
};
