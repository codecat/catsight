#pragma once

#include <Common.h>
#include <Tab.h>
#include <Tasks/Task.h>

class TaskWaitTab : public Tab
{
public:
	Task* m_task = nullptr;

public:
	TaskWaitTab(Inspector* inspector, const s2::string& id);
	virtual ~TaskWaitTab();

	virtual void BeginTask(Task* task);
	virtual void BeginTask(const Task::Func& func, void* userdata = nullptr);
	virtual void TaskFinished();

	virtual bool CanClose() override;

	virtual void Render(float dt) override;
};
