#include <Common.h>
#include <Tasks/Task.h>

Task::Task(const Func& func, void* userdata)
{
	m_func = func;
	m_userdata = userdata;
}

Task::~Task()
{
}

void Task::RunSync()
{
	m_func(m_userdata);
}

void Task::Then(const CallbackFunc& func)
{
	m_callback = func;
}

bool Task::HasCallback()
{
	return m_callback != nullptr;
}

void Task::RunCallback()
{
	m_callback(this);
}
