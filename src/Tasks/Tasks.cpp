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

void Tasks::Run(Task* task)
{
	std::scoped_lock lock(m_updateLock);
	m_queuedTasks.insert(0, task);
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
