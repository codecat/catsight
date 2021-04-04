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

	m_keepRunning = false;
	m_thread.join();
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
	std::scoped_lock lock(m_tasks->m_updateLock);

	if (m_tasks->m_queuedTasks.len() > 0) {
		m_task = m_tasks->m_queuedTasks.pop();
	} else {
		m_task = nullptr;
	}
}

void TaskWorker::Update()
{
	while (m_keepRunning) {
		System::Sleep(30);

		if (m_task == nullptr) {
			FindNewTask();
		}

		while (m_task != nullptr) {
			Task* task = m_task;
			task->RunSync();

			if (task->HasCallback()) {
				std::scoped_lock lock(m_tasks->m_updateLock);
				m_tasks->m_finishedTasks.add(task);
			} else {
				delete task;
			}

			FindNewTask();
		}
	}
}
