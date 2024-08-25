#pragma once
#include "System.h"
#include "System.Collections.h"
#include "Atomic.h"
#include "CondVar.h"
#include "Mutex.h"
#include "Thread.h"

using namespace System;
using namespace System::Collections::Generic;

namespace System::Threading
{    
    #define TJob template<typename T>

    struct Job
    {
        virtual void Execute() { }
    };

    TJob class ThreadPoolBase
    {
    public:
        void Start() 
        {
            for (var i = 0; i < threadCount; i++)
            {
                var thread = Thread<ThreadPoolBase<T>*>::Run(ThreadLoop, this);
                threads.Add(thread);
            }

            waitOneCond.WakeAll();
        }

        void Stop()
        {
            //set termination flag and notify all threads
            queueMutex.Lock();
            shouldTerminate = true;
            queueMutex.Unlock();

            queueCond.WakeAll();

            //wait all threads to finish
            Thread<>::WaitAll(threads, true);
            threads.Clear();
        }

        void QueueJob(T job, bool waitIfAllThreadsBusy = false)
        {
            queueMutex.Lock();
            jobs.Enqueue(job);
            runningCount++;
            queueMutex.Unlock();

            while (waitIfAllThreadsBusy && AreAllThreadsBusy())
                waitOneCond.Wait(waitOneMutex);

            queueCond.Wake();
        }

        bool IsBusy()
        {
            queueMutex.Lock();
            var isBusy = runningCount != 0;
            queueMutex.Unlock();

            return isBusy;
        }

        void WaitAll()
        {
            while (IsBusy())
                waitOneCond.Wait(waitOneMutex);
        }

        int ThreadCount()
        {
            return (int)threads.Count();
        }

        ~ThreadPoolBase()
        {
            Stop();
        }

        static void SetThreadCount(int count)
        {
            if (count < 1 || count > 4096)
                throw NotSupportedException((string)"Thread count must be in range [1..4096].");

            threadCount = count;
        }

    private:
        inline static int threadCount = Environment::ProcessorCount();

        bool shouldTerminate = false;
        Mutex queueMutex;
        CondVar queueCond;
        List<ThreadBase*> threads;
        Queue<T> jobs;

        int runningCount = 0;
        Mutex waitOneMutex;
        CondVar waitOneCond;

        bool AreAllThreadsBusy()
        {
            queueMutex.Lock();
            var isBusy = runningCount >= threads.Count();
            queueMutex.Unlock();

            return isBusy;
        }

        static void ThreadLoop(ThreadPoolBase<T>* pool)
        {
            while (true)
            {
                //wait for job or termination
                pool->queueMutex.Lock();

                while (pool->jobs.Count() == 0 && pool->shouldTerminate == false)
                    pool->queueCond.Wait(pool->queueMutex);

                if (pool->shouldTerminate)
                {
                    pool->queueMutex.Unlock();
                    break;
                }

                var job = pool->jobs.Dequeue();
                pool->queueMutex.Unlock();

                //execute job
                job.Execute();

                //notify that we are finished with one thread
                pool->queueMutex.Lock();      
                pool->runningCount--;
                pool->queueMutex.Unlock();

                pool->waitOneCond.WakeAll();
            }

            //notify that we are finished if the pool is stopped
            pool->waitOneCond.WakeAll();
        }
    };


    struct JobNoArg: Job
    {
        using Func = void (*)(void);

        JobNoArg(Func func):
            Function(func)
        { }

        Func Function;   

        void Execute()
        {
            Function();
        }  
    };

    template<typename...> 
    class ThreadPool: public ThreadPoolBase<JobNoArg>
    {
        public:
            using Func = void (*)(void);

            void QueueJob(Func func, bool waitIfAllThreadsBusy = false)
            {
                var job = JobNoArg(func);
                ThreadPoolBase<JobNoArg>::QueueJob(job, waitIfAllThreadsBusy);
            }
    };



    TC struct JobWithArg: Job
    {
        using Func = void(*)(T arg);

        JobWithArg(Func func, T arg):
            Function(func), Arg(arg)
        { }

        Func Function;
        T Arg;  

        void Execute()
        {
            Function(Arg);
        }       
    };

    TC class ThreadPool<T>: public ThreadPoolBase<JobWithArg<T>>
    {
        public:
            using Func = void(*)(T arg);

            void QueueJob(Func func, T arg, bool waitIfAllThreadsBusy = false)
            {
                var job = JobWithArg<T>(func, arg);
                ThreadPoolBase<JobWithArg<T>>::QueueJob(job, waitIfAllThreadsBusy);
            }
    };
}