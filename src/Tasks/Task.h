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
	std::atomic<float> m_durationMilliseconds;
	void* m_userdata = nullptr;

private:
	Func m_func;
	CallbackFunc m_callback;
	std::atomic<bool> m_canceled = false;

public:
	Task(const Func& func, void* userdata = nullptr);
	~Task();

	void RunSync();

	Task* Then(const CallbackFunc& func);

	void Cancel();
	bool IsCanceled();

	bool HasCallback();
	void RunCallback();
};
