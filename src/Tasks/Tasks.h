#pragma once

#include <Common.h>

#include <Tasks/TaskWorker.h>
#include <Tasks/Task.h>
#include <mutex>

class Tasks
{
	friend class TaskWorker;

private:
	std::mutex m_updateLock;

	s2::list<TaskWorker*> m_workers;
	s2::list<Task*> m_queuedTasks;
	s2::list<Task*> m_finishedTasks;

public:
	Tasks();
	~Tasks();

	TaskWorker* AddWorker();
	int GetWorkerCount();
	int GetActiveWorkerCount();

	void Run(Task* task);

	void Update();
};
