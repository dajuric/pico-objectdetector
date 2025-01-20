#pragma once
#include "System.h"

//#include <stdatomic.h>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <pthread.h>
#include <unistd.h>
#endif

namespace System::Threading
{
    class Mutex
    {
    public:
        Mutex()
        {
            thd_thread_mutex_init(&lock);
        }
        
        ~Mutex()
        {
            thd_thread_mutex_destroy(&lock);
        }

        void Lock()
        {
            thd_thread_mutex_lock(&lock);
        }

        void Unlock()
        {
            thd_thread_mutex_unlock(&lock);
        }

        void* Ptr()
        {
            return &lock;
        }

    private:
#ifdef _WIN32
        typedef CRITICAL_SECTION thd_mutex_t;

        void thd_thread_mutex_init(thd_mutex_t* mutex)
        {
            InitializeCriticalSection(mutex);
        }

        void thd_thread_mutex_destroy(thd_mutex_t* mutex)
        {
            DeleteCriticalSection(mutex);
        }

        void thd_thread_mutex_lock(thd_mutex_t* mutex)
        {
            EnterCriticalSection(mutex);
        }

        void thd_thread_mutex_unlock(thd_mutex_t* mutex)
        {
            LeaveCriticalSection(mutex);
        }

#else
        typedef pthread_mutex_t thd_mutex_t;

        void thd_thread_mutex_init(thd_mutex_t* mutex)
        {
            pthread_mutex_init(mutex, NULL);
        }

        void thd_thread_mutex_destroy(thd_mutex_t* mutex)
        {
            pthread_mutex_destroy(mutex);
        }

        void thd_thread_mutex_lock(thd_mutex_t* mutex)
        {
            pthread_mutex_lock(mutex);
        }

        void thd_thread_mutex_unlock(thd_mutex_t* mutex)
        {
            pthread_mutex_unlock(mutex);
        }

#endif

        thd_mutex_t lock;
    };
}