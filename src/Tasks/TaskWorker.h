#pragma once

#include <Common.h>

#include <Tasks/Task.h>

#include <thread>
#include <atomic>

class Tasks;

class TaskWorker
{
private:
	Tasks* m_tasks;

	std::thread m_thread;
	std::atomic<bool> m_idle;
	std::atomic<Task*> m_task;

public:
	TaskWorker(Tasks* tasks);
	~TaskWorker();

	bool IsIdle();

	void GiveTask(Task* task);

private:
	void Update();
};
