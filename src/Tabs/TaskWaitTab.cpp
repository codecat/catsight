#include <Common.h>
#include <Tabs/TaskWaitTab.h>
#include <Inspector.h>

#include <hello_imgui.h>

TaskWaitTab::TaskWaitTab(Inspector* inspector, const s2::string& id)
	: Tab(inspector, id)
{
}

TaskWaitTab::~TaskWaitTab()
{
}

void TaskWaitTab::BeginTask(Task* task)
{
	m_task = task;
	m_task->Then([this](Task*) {
		TaskFinished();
	});
	m_inspector->m_tasks.Run(m_task);
}

void TaskWaitTab::BeginTask(const Task::Func& func, void* userdata)
{
	BeginTask(new Task(func, userdata));
}

void TaskWaitTab::TaskFinished()
{
	m_taskDuration = m_task->m_durationMilliseconds;
	m_task = nullptr;
}

bool TaskWaitTab::CanClose()
{
	return m_task == nullptr;
}

void TaskWaitTab::Render(float dt)
{
	if (m_task != nullptr) {
		if (ImGui::Button("Cancel")) {
			m_task->Cancel();
		}
		ImGui::SameLine();
		ImGui::ProgressBar(m_task->m_progress);
	} else {
		ImGui::Text("(Task duration: %g ms)", m_taskDuration);
	}
}
