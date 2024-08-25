#pragma once
#include "Definitions.h"

namespace System
{
    TC class SPtr
    {
    public:
        SPtr()
            :SPtr(null)
        {}

        SPtr(T* ptr)
        {
            this->ptr = ptr;
            this->cPtr = new UInt32(1);
        }

        SPtr(const SPtr<T>& p)
        {
            ptr = p.ptr;
            cPtr = p.cPtr;
            (*cPtr)++;
        }

        ~SPtr()
        {
            (*cPtr)--;
            Dispose();
        }

        SPtr<T>& operator = (T* p)
        {
            (*cPtr)--;
            Dispose();

            ptr = p;
            cPtr = new UInt32(1);

            return *this;
        }

        SPtr<T>& operator = (const SPtr<T>& p)
        {
            (*cPtr)--;
            Dispose();

            ptr = p.ptr;
            cPtr = p.cPtr;
            (*cPtr)++;

            return *this;
        }

        T& operator * () { return *ptr; }
        T* operator -> () { return ptr; }
        T& operator [] (int i) { return ptr[i]; }

        bool operator != (SPtr<T>& p) { return ptr != p.ptr; }
        bool operator == (SPtr<T>& p) { return ptr == p.ptr; }

        T* Ptr()
        {
            return ptr;
        }

        UInt32 RefCount()
        {
            return *cPtr;
        }

    private:
        T* ptr;
        UInt32* cPtr; //TODO: make thread safe via Atomic<>

        void Dispose()
        {
            if (*cPtr > 0)
                return;

            delete ptr; ptr = null;
            delete cPtr; cPtr = null;
        }
    };

    TC class SPtr<T[]>
    {
    public:
        SPtr(T* ptr)
        {
            throw NotImplementedException("SPtr for arrays is not implemented or not supported.");
        }
    };
}