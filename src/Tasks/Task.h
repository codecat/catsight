#pragma once

#include <Common.h>

#include <atomic>

class Task
{
public:
	typedef s2::func<void(Task*)> Func;
	typedef s2::func<void(Task*)> CallbackFunc;

public:
	std::atomic<float> m_progress;
	void* m_userdata = nullptr;

private:
	Func m_func;

	CallbackFunc m_callback;

public:
	Task(const Func& func, void* userdata);
	~Task();

	void RunSync();

	Task* Then(const CallbackFunc& func);

	bool HasCallback();
	void RunCallback();
};
