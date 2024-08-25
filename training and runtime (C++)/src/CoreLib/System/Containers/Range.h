#pragma once
#include "Definitions.h"

namespace System
{
    TC struct Range
    {
        Range()
        { }

        Range(T start, T stop):
            Start(start), Stop(stop)
        { }
    
        T Start;
        T Stop;
    };	
}