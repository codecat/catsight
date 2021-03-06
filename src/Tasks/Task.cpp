#include <Common.h>
#include <Tasks/Task.h>
#include <Chrono.h>

Task::Task(const Func& func, void* userdata)
{
	m_func = func;
	m_userdata = userdata;

	m_progress = 0.0f;
	m_durationMilliseconds = 0.0f;
}

Task::~Task()
{
}

void Task::RunSync()
{
	auto tmStart = Chrono::Now();
	m_func(this);
	m_durationMilliseconds = Chrono::MillisecondsSince(tmStart);
}

Task* Task::Then(const CallbackFunc& func)
{
	m_callback = func;
	return this;
}

void Task::Cancel()
{
	m_canceled = true;
}

bool Task::IsCanceled()
{
	return m_canceled;
}

bool Task::HasCallback()
{
	return m_callback != nullptr;
}

void Task::RunCallback()
{
	m_callback(this);
}
