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

TaskWorker* Tasks::GetAvailableWorker()
{
	for (auto worker : m_workers) {
		if (worker->IsIdle()) {
			return worker;
		}
	}
	return nullptr;
}

Task* Tasks::Run(const Task::Func& func, void* userdata)
{
	auto newTask = new Task(func, userdata);
	m_queuedTasks.insert(0, newTask);
	return newTask;
}

void Tasks::Update()
{
	m_updateLock.lock();

	while (m_queuedTasks.len() > 0) {
		auto worker = GetAvailableWorker();
		if (worker == nullptr) {
			break;
		}
		worker->GiveTask(m_queuedTasks.pop());
	}

	while (m_finishedTasks.len() > 0) {
		auto task = m_finishedTasks.pop();
		task->RunCallback();
		delete task;
	}

	m_updateLock.unlock();
}
