#include <Common.h>
#include <Tabs/TaskWaitTab.h>

#include <hello_imgui.h>

TaskWaitTab::TaskWaitTab(Inspector* inspector, const s2::string& id)
	: Tab(inspector, id)
{
}

TaskWaitTab::~TaskWaitTab()
{
}

void TaskWaitTab::TaskFinished()
{
	m_task = nullptr;
}

bool TaskWaitTab::CanClose()
{
	return m_task == nullptr;
}

void TaskWaitTab::Render(float dt)
{
	if (m_task != nullptr) {
		ImGui::ProgressBar(m_task->m_progress);
	}
}
