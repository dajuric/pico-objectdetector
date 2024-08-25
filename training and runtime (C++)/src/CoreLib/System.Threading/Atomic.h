#pragma once
#include "System.h"
#include "Mutex.h"

//#include <stdatomic.h>

namespace System::Threading
{
    TC class Atomic
    {
    public:
        Atomic()
            :Atomic(0)
        {}

        Atomic(T value)
            :value(value)
        { }

        ~Atomic()
        {
            //lock.~Mutex(); //TODO: is this needed ? no
        }

        T Add(const T amount)
        {
            lock.Lock();
            this->value += amount;
            lock.Unlock();

            return this->value;
        }

        T Sub(const T amount)
        {
            lock.Lock();
            this->value -= amount;
            lock.Unlock();

            return this->value;
        }

        T operator++(int)
        {
            return Add((T)1);
        }

        T operator--(int)
        {
            return Sub((T)1);
        }

        operator T()
        {
            return this->value;
        }

    private:
        volatile T value;
        Mutex lock;
    };
}