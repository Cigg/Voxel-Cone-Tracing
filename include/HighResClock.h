#ifndef HIGHRESCLOCK_H
#define HIGHRESCLOCK_H

#include <chrono>
#if WIN32
#include <Windows.h>
#endif

namespace timer
{
#ifdef WIN32

	struct HighResClock {
		typedef long long                               rep;
		typedef std::nano                               period;
		typedef std::chrono::duration<rep, period>      duration;
		typedef std::chrono::time_point<HighResClock>   time_point;
		static const bool is_steady = true;

		static time_point now();
	};


	namespace
	{
		const long long g_Frequency = []() -> long long {
			LARGE_INTEGER frequency;
			QueryPerformanceFrequency(&frequency);
			return frequency.QuadPart;
		}();
	}

#else

	typedef std::chrono::high_resolution_clock HighResClock;

#endif

	HighResClock::time_point now();
}

#endif // HIGHRESCLOCK_H