#include <Common.h>
#include <Tasks/TaskWorker.h>
#include <Tasks/Tasks.h>

#include <System.h>

TaskWorker::TaskWorker(Tasks* tasks)
{
	m_tasks = tasks;

	m_task = nullptr;

	m_thread = std::thread([this]() {
		Update();
	});
}

TaskWorker::~TaskWorker()
{
	if (m_task != nullptr) {
		delete m_task;
	}
}

bool TaskWorker::IsIdle()
{
	return m_task == nullptr;
}

void TaskWorker::GiveTask(Task* task)
{
	assert(m_task == nullptr);
	m_task = task;
}

void TaskWorker::FindNewTask()
{
	m_tasks->m_updateLock.lock();

	if (m_tasks->m_queuedTasks.len() > 0) {
		m_task = m_tasks->m_queuedTasks.pop();
	} else {
		m_task = nullptr;
	}

	m_tasks->m_updateLock.unlock();
}

void TaskWorker::Update()
{
	while (true) {
		System::Sleep(50);

		if (m_task == nullptr) {
			FindNewTask();
		}

		while (m_task != nullptr) {
			Task* task = m_task;
			task->RunSync();

			if (task->HasCallback()) {
				m_tasks->m_updateLock.lock();
				m_tasks->m_finishedTasks.add(task);
				m_tasks->m_updateLock.unlock();
			} else {
				delete task;
			}

			FindNewTask();
		}
	}
}
