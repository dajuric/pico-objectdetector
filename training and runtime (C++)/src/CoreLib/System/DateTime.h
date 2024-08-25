#pragma once
#include "Definitions.h"
#include "String_.h"
#include <time.h>

namespace System
{
	class DateTime
	{
	public:
		int Year;
		int Month;
		int Day;

		int Hours;
		int Minutes;
		int Seconds;

		static DateTime Now()
		{
			time_t t = time(NULL);
			struct tm* tm = localtime(&t);

			var dt = DateTime();
			dt.Year = tm->tm_year;
			dt.Month = tm->tm_mon;
			dt.Day = tm->tm_mday;

			dt.Hours = tm->tm_hour;
			dt.Minutes = tm->tm_min;
			dt.Seconds = tm->tm_sec;

			return dt;
		}

		//format explanation: https://www.tutorialspoint.com/c_standard_library/c_function_strftime.htm
		string Format(const string format = "%y-%m-%d %H:%M:%S")
		{
			struct tm tm;
			tm.tm_year = this->Year;
			tm.tm_mon = this->Month;
			tm.tm_mday = this->Day;

			tm.tm_hour = this->Hours;
			tm.tm_min = this->Minutes;
			tm.tm_sec = this->Seconds;

			char s[64];
			size_t ret = strftime(s, sizeof(s), format.Ptr(), &tm);
			return String(s);
		}

	private:
		DateTime()
			:Year(0), Month(0), Day(0), Hours(0), Minutes(0), Seconds(0)
		{ }

	};
}