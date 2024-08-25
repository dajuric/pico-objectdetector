#pragma once
#include "Definitions.h"

#if !defined(_INITIALIZER_LIST) && !defined(_INITIALIZER_LIST_)
#define _INITIALIZER_LIST
#define _INITIALIZER_LIST_

namespace std
{
    TC class initializer_list
    {
    public:
        constexpr initializer_list(const T* first, const T* last):
            First(first), Last(last)
        { }

        const T* begin() const { return this->First; } 
        const T* end() const { return this->Last; }
        long size() const { return this->Last - this->First; }

        const T* First;
        const T* Last;
        long Length() { this->Last - this->First; }
    };
}
#endif

TC using InitializerList = std::initializer_list<T>;