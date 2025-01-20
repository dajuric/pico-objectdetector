#pragma once
#include "Definitions.h"

#include <time.h>

namespace System
{
    //the idea is taken from: https://rosettacode.org/wiki/Linear_congruential_generator#C
    class Random
    {
    public:
        Random()
        {
            this->next = (UInt32)time(null);
        }

        Random(UInt32 seed)
        {
            this->next = seed;
        }

        int Next(int min, int max)
        {
            var rawValue = NextRaw();

            var value = (double)(max - min) * rawValue / (RANDOM_MAX - 0) + min;
            return (int)value;
        }

        double NextDouble()
        {
            var rawValue = NextRaw();

            var value = (double)rawValue / RANDOM_MAX;
            return value;
        }

    private:
        const int RANDOM_MAX = (1U << 31) - 1;
        UInt32 next;

        UInt32 NextRaw()
        {
            this->next = (this->next * 1103515245 + 12345) & RANDOM_MAX;
            return this->next;
        }
    };
}