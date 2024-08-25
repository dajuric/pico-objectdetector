#pragma once
#include "Definitions.h"

namespace System
{
    TC class UPtr
    {
    public:
        UPtr()
            :ptr(null)
        {}

        UPtr(T* ptr)
            :ptr(ptr)
        { }

        UPtr(UPtr<T>&& p) noexcept
            :ptr(p.Release())
        { }

        ~UPtr()
        {
            delete this->ptr;
        }

        UPtr<T>& operator = (T* p)
        {
            delete this->ptr;
            this->ptr = p;

            return *this;
        }

        UPtr(const UPtr<T>& p) = delete;
        UPtr<T>& operator = (const UPtr<T>& p) = delete;

        T& operator * () { return *ptr; }
        T* operator -> () { return ptr; }
        T& operator [] (int i) { return ptr[i]; }

        bool operator != (UPtr<T>& p) { return ptr != p.ptr; }
        bool operator == (UPtr<T>& p) { return ptr == p.ptr; }

        T* Ptr()
        {
            return ptr;
        }

        T* Release()
        {
            var t = ptr;
            ptr = null;
            return t;
        }

    private:
        T* ptr;
    };

    TC class UPtr<T[]>
    {
    public:
        UPtr(T* ptr)
        {
            throw NotImplementedException("UPtr for arrays is not implemented or not supported.");
        }
    };
}