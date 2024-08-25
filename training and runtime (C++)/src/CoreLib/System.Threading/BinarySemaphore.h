#pragma once
#include "System.h"
#include "Mutex.h"
#include "CondVar.h"

namespace System::Threading
{
    //the idea is taken from: https://stackoverflow.com/questions/7478684/how-to-initialise-a-binary-semaphore-in-c/7478825#7478825
    class BinarySemaphore
    {
    public:
        BinarySemaphore(bool released):
            released(released)
        { }

        ~BinarySemaphore()
        {
            ReleaseAll();
        }

        void Wait()
        {
            mutex.Lock();

            while (released == false)
                cond.Wait(mutex);

            released = false;

            mutex.Unlock();
        }

        void Release()
        {
            mutex.Lock();

            released = true;
            cond.Wake();

            mutex.Unlock();
        }

        void ReleaseAll()
        {
            mutex.Lock();

            released = true;
            cond.WakeAll();

            mutex.Unlock();
        }

    private:
        Mutex mutex;
        CondVar cond;
        bool released;
    };
}