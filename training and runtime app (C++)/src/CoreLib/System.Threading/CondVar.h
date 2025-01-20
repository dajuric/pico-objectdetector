#pragma once
#include "System.h"
#include "Mutex.h"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <pthread.h>
#include <unistd.h>
#endif

namespace System::Threading
{
    class CondVar
    {
    public:
        CondVar()
        {
            thd_condition_init(&cond);
        }

        ~CondVar()
        {
            thd_condition_destroy(&cond);
        }

        void Wake()
        {
            thd_condition_signal(&cond);
        }

        void WakeAll()
        {
            thd_condition_broadcast(&cond);
        }

        void Wait(Mutex& mutex)
        {
            thd_condition_wait(&cond, (thd_mutex_t*)mutex.Ptr());
        }

    private:
#ifdef _WIN32
        typedef CONDITION_VARIABLE thd_cond_t;
        typedef CRITICAL_SECTION   thd_mutex_t;

        int thd_condition_init(thd_cond_t* cond)
        {
            InitializeConditionVariable(cond);
            return 0;
        }

        int thd_condition_destroy(thd_cond_t* cond)
        {
            return 0;
        }

        int thd_condition_signal(thd_cond_t* cond)
        {
            WakeConditionVariable(cond);
            return 0;
        }

        int thd_condition_broadcast(thd_cond_t* cond)
        {
            WakeAllConditionVariable(cond);
            return 0;
        }

        int thd_condition_wait(thd_cond_t* cond, thd_mutex_t* mutex)
        {
            return SleepConditionVariableCS(cond, mutex, INFINITE) == false;
        }

#else
        typedef pthread_cond_t  thd_cond_t;
        typedef pthread_mutex_t thd_mutex_t;

        int thd_condition_init(thd_cond_t* cond)
        {
            return pthread_cond_init(cond, NULL);
        }

        int thd_condition_destroy(thd_cond_t* cond)
        {
            return pthread_cond_destroy(cond);
        }

        int thd_condition_signal(thd_cond_t* cond)
        {
            return pthread_cond_signal(cond);
        }

        int thd_condition_broadcast(thd_cond_t* cond)
        {
            return pthread_cond_broadcast(cond);
        }

        int thd_condition_wait(thd_cond_t* cond, thd_mutex_t* mutex)
        {
            return pthread_cond_wait(cond, mutex);
        }

#endif

        thd_cond_t cond;
    };
}