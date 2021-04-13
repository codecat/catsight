#pragma once

#include <Common.h>
#include <Chrono.h>
#include <Inspector.h>
#include <System/ProcessInfo.h>

#include <hello_imgui.h>

class Explorer
{
public:
	static Explorer* Instance;

private:
	HelloImGui::ImageGlPtr m_imgBackground;
	HelloImGui::RunnerParams m_params;

	Chrono::Time m_lastFrame;

	s2::list<ProcessInfo> m_processes;
	UserInfo m_currentUser;

	bool m_metricsVisible = false;

public:
	s2::list<Inspector*> m_inspectors;

public:
	void Run();

	void SetStyle();
	void LoadFonts();

	void BeginApp();
	void EndApp();

	Inspector* GetInspector(const ProcessInfo& info);
	Inspector* GetInspector(int pid);

	void RenderMenu(float dt);
	void Render();
};
