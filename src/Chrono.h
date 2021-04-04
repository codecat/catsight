#pragma once

#include <Common.h>

#include <chrono>

namespace Chrono
{
	typedef std::chrono::high_resolution_clock Clock;
	typedef Clock::time_point Time;

	Time Now();

	float DurationMilliseconds(Time a, Time b);
	float MillisecondsSince(Time t);
}
