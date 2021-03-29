#pragma once

#include <Common.h>

class Task
{
public:
	typedef s2::func<void(void*)> Func;
	typedef s2::func<void(Task*)> CallbackFunc;

private:
	Func m_func;
	void* m_userdata;

	CallbackFunc m_callback;

public:
	Task(const Func& func, void* userdata);
	~Task();

	void RunSync();

	void Then(const CallbackFunc& func);

	bool HasCallback();
	void RunCallback();
};
