#include <Common.h>
#include <Chrono.h>

Chrono::Time Chrono::Now()
{
	return Clock::now();
}

float Chrono::DurationMilliseconds(Time a, Time b)
{
	return std::chrono::duration_cast<std::chrono::microseconds>(b - a).count() / 1000.0f;
}
