#pragma once

#include "Exception.h"
using namespace System;

#include <malloc.h>
#include <string.h> //memory.h

inline
void* operator new(UInt64 size)
{
    void* p = malloc(size);
    if (p == null)
        throw IOException("Out of memory."); //for Windows only, Linux uses OOM

    memset(p, 0, size);
    return p;
}

inline
void operator delete(void* p) noexcept
{
    if (p == null)
        return;
        
    free(p);
}

inline
void* operator new[](UInt64 size)
{
    void* p = malloc(size);
    if (p == null)
        throw IOException("Out of memory."); //for Windows only, Linux uses OOM

    memset(p, 0, size);
    return p;
}

inline
void operator delete[](void* p) noexcept
{
    if (p == null)
        return;

    free(p);
}

