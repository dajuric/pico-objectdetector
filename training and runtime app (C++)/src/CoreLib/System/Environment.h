#pragma once
#include "Definitions.h"
#include "String_.h"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "psapi.h"
#else
#include <unistd.h>
#include <sys/resource.h>
#endif

namespace System
{
    class Environment
    {
    public:
        static int ProcessorCount()
        {
            int count = -1;

#ifdef _WIN32
            SYSTEM_INFO info;
            GetSystemInfo(&info);
            count = info.dwNumberOfProcessors;
#else
            count = sysconf(_SC_NPROCESSORS_CONF);
#endif

            return count;
        }

        static UInt64 TotalSystemMemory()
        {
            UInt64 totalSize = -1;

#ifdef _WIN32
            MEMORYSTATUSEX status;
            status.dwLength = sizeof(status);
            GlobalMemoryStatusEx(&status);
            totalSize = status.ullTotalPhys;
#else
            long pages = sysconf(_SC_PHYS_PAGES);
            long page_size = sysconf(_SC_PAGE_SIZE);
            totalSize = pages * page_size;
#endif

            return totalSize;
        }

        //https://stackoverflow.com/questions/63166/how-to-determine-cpu-and-memory-consumption-from-inside-a-process
        static UInt64 ProcessWorkingSet()
        {
            UInt64 workingSet = -1;
            
#ifdef _WIN32
            PROCESS_MEMORY_COUNTERS_EX pmc;
            GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
            SIZE_T virtualMemUsedByMe = pmc.PrivateUsage;
            workingSet = pmc.WorkingSetSize;
#else
            //struct rusage r_usage;
            //getrusage(RUSAGE_SELF, &r_usage);
            //workingSet = r_usage.ru_maxrss * 1024;

            FILE* file = fopen("/proc/self/status", "r");
            int result = -1;
            char line[255];

            while (fgets(line, 255, file) != NULL) 
            {
                char field[] = "VmRSS:"; //previously VmSize:
                if (strncmp(line, field, strlen(field)) == 0) 
                {
                    result = ParseProcStatusValue(line);
                    break;
                }
            }
            fclose(file);

            workingSet = result * 1024;
#endif

            return workingSet;
        }

        const string NewLine = "\n";

    private:
        static int ParseProcStatusValue(char* line)
        {
            // This assumes that a digit will be found and the line ends in " Kb".
            int i = (int)strlen(line);
            const char* p = line;

            while (*p < '0' || *p > '9')
                p++;

            line[i - 3] = '\0';
            i = atoi(p);

            return i;
            }
    };
}