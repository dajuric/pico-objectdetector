#pragma once
#include "Definitions.h"

#define TItTEl template<typename TIterator, typename TElem>

namespace System
{
    TItTEl abstract class Enumerable
    {
    public:
        virtual TIterator begin() = 0;

        virtual TIterator end() = 0;
    };
}