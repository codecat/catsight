#include <Common.h>
#include <Tasks/Tasks.h>

Tasks::Tasks()
{
	printf("Adding 4 workers\n");
	for (int i = 0; i < 4; i++) {
		AddWorker();
	}
}

Tasks::~Tasks()
{
	for (auto worker : m_workers) {
		delete worker;
	}
}

TaskWorker* Tasks::AddWorker()
{
	auto newWorker = new TaskWorker(this);
	m_workers.add(newWorker);
	return newWorker;
}

int Tasks::GetWorkerCount()
{
	return (int)m_workers.len();
}

int Tasks::GetActiveWorkerCount()
{
	int ret = 0;
	for (auto worker : m_workers) {
		if (!worker->IsIdle()) {
			ret++;
		}
	}
	return ret;
}

Task* Tasks::Run(const Task::Func& func, void* userdata)
{
	auto newTask = new Task(func, userdata);
	m_updateLock.lock();
	m_queuedTasks.insert(0, newTask);
	m_updateLock.unlock();
	return newTask;
}

void Tasks::Update()
{
	m_updateLock.lock();

	while (m_finishedTasks.len() > 0) {
		auto task = m_finishedTasks.pop();
		task->RunCallback();
		delete task;
	}

	m_updateLock.unlock();
}
