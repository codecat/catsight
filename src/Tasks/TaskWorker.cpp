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

void TaskWorker::Update()
{
	while (true) {
		System::Sleep(1);

		if (m_task != nullptr) {
			Task* task = m_task;
			task->RunSync();

			if (task->HasCallback()) {
				m_tasks->m_updateLock.lock();
				m_tasks->m_finishedTasks.add(task);
				m_tasks->m_updateLock.unlock();
			} else {
				delete task;
			}

			m_task = nullptr;
		}
	}
}
