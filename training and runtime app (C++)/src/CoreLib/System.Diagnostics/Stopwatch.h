#pragma once
#include "System.h"
#include <time.h>

namespace System::Diagnostics
{
#define SEC_TO_MS(sec) ((sec)*1000)
#define NS_TO_MS(ns)    ((ns)/1000000)

	class Stopwatch
	{
	public:
		static UInt64 TotalMilliseconds()
		{
			struct timespec ts;
			bool ok = timespec_get(&ts, TIME_UTC) == TIME_UTC;
			
			if (!ok)
				throw Exception("Error getting elapsed time.");

			UInt64 ms = SEC_TO_MS((UInt64)ts.tv_sec) + NS_TO_MS((UInt64)ts.tv_nsec);
			return ms;
		}

		static double TotalSeconds()
		{
			return (double)TotalMilliseconds() / 1000;
		}
	};
}