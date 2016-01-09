#include "HighResClock.h"

#if WIN32
#include <Windows.h>

namespace
{
	const long long g_Frequency = []() -> long long {
		LARGE_INTEGER frequency;
		QueryPerformanceFrequency(&frequency);
		return frequency.QuadPart;
	}();
}

timer::HighResClock::time_point timer::HighResClock::now() {
	LARGE_INTEGER count;
	QueryPerformanceCounter(&count);
	return time_point(duration(count.QuadPart * static_cast<rep>(period::den) / g_Frequency));
}
#endif

timer::HighResClock::time_point timer::now() {
	return timer::HighResClock::now();
}