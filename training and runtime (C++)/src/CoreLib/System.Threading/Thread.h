#pragma once
#include "System.h"
#include "System.Collections.h"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <pthread.h>
#include <unistd.h>
#endif

using namespace System;
using namespace System::Collections::Generic;

//some constructs are taken from and modified: https://github.com/aseprite/tinythreadpp/blob/master/source/tinythread.h
namespace System::Threading
{
    typedef void* (*thdStartMethod)(void*);
    typedef void OnThreadError(Exception& ex);

    abstract class ThreadBase
    {
    private:
#ifdef _WIN32
        typedef HANDLE thd_thread;

        typedef struct
        {
            thdStartMethod i_method;
            void* i_data;
        }
        t_internal_parameters;

        static DWORD WINAPI internal_method_ptr(LPVOID arg)
        {
            t_internal_parameters* params = (t_internal_parameters*)arg;
            params->i_method(params->i_data);
            free(params);
            return 0;
        }

        int thd_thread_create(thd_thread* thread, thdStartMethod method, void* data)
        {
            t_internal_parameters* params = (t_internal_parameters*)malloc(sizeof(t_internal_parameters));
            if (params)
            {
                params->i_method = method;
                params->i_data = data;
                *thread = CreateThread(NULL, 0, internal_method_ptr, params, 0, NULL);
                if (*thread == NULL)
                {
                    free(params);
                    return 1;
                }
                return 0;
            }
            return 1;
        }

        int thd_thread_join(thd_thread* thread)
        {
            if (WaitForSingleObject(*thread, INFINITE) != WAIT_FAILED)
            {
                if (CloseHandle(*thread))
                {
                    return 0;
                }
            }
            return 1;
        }

        int thd_gettid(thd_thread* thread)
        {
            return (int)GetThreadId(*thread);
        }

        static int thd_thread_self()
        {
            return (int)GetCurrentThreadId();
        }

        static void thd_sleep(Int32 milliseconds)
        {
            Sleep(milliseconds);
        }

#else
        typedef pthread_t thd_thread;

        int thd_thread_create(thd_thread* thread, thdStartMethod method, void* data)
        {
            return pthread_create(thread, NULL, method, data);
        }

        int thd_thread_join(thd_thread* thread)
        {
            return pthread_join(*thread, NULL);
        }

        int thd_gettid(thd_thread* thread)
        {
            return (int)(*thread); //TODO: can we do that ?
        }

        static int thd_thread_self()
        {
            return (int)pthread_self();
        }

        static void thd_sleep(Int32 milliseconds)
        {
            struct timespec ts;
            ts.tv_sec = milliseconds / 1000;
            ts.tv_nsec = (milliseconds % 1000) * 1000000;
            nanosleep(&ts, NULL);
        }

#endif

    public:
        ThreadBase(const ThreadBase& other) = delete;

        ThreadBase& operator = (const ThreadBase&) = delete;

        virtual void Join() = 0;

        virtual ~ThreadBase()
        { }

        int ThreadId()
        {
            if (this->joined)
                return -1;

            return thd_gettid(&threadPtr);
        }

        static int CallingThreadId()
        {
            return thd_thread_self();
        }

        static void SleepFor(UInt32 milliseconds)
        {
            thd_sleep(milliseconds);
        }

        static void WaitAll(Collection<ThreadBase*>& threads, bool destroyThread = true)
        {
            for (var i = 0; i < threads.Count(); i++)
            {
                var thread = threads[i];
                thread->Join();

                if (destroyThread)
                    delete thread;
            }
        }

        static void SubscribeErrorHandler(OnThreadError onError)
        {
            if (onError == null)
                throw ArgumentException("Error handler can not be null.");

            errorHandlers.Add(onError);
        }

    protected:
        ThreadBase()
            :threadPtr(), joined(false)
        {}

        virtual void Start() = 0;

        void StartThread(thdStartMethod rawStartMethod, void* args)
        {
            int status = thd_thread_create(&this->threadPtr, rawStartMethod, args);
            if (status != 0)
            {
                throw Exception("Thread create error");
            }
        }

        void JoinThread()
        {
            if (this->joined)
                return;

            var status = thd_thread_join(&this->threadPtr);
            this->joined = true;

            if (status != 0)
            {
                throw Exception("Thread join error");
            }
        }

        static void NotifyErrorHandlers(Exception& ex)
        {
            for (var handler : errorHandlers)
                handler(ex);
        }

        thd_thread threadPtr;
        bool joined;
        inline static List<OnThreadError*> errorHandlers;
    };


    template<typename...> 
    class Thread: public ThreadBase
    {
    public:
        using ThreadStartMethod = void* (*)(void);

        void Join() override
        {
            JoinThread();

            if (ctx.IsExceptionThrown)
                throw ctx.Exception;
        }

        ~Thread()
        {
            Join();
        }

        static Thread* Run(ThreadStartMethod method)
        {
            var thread = new Thread(method);
            thread->Start();
            return thread;
        }

    private:
        struct ThreadContext
        {
            ThreadContext(ThreadStartMethod func)
                :Func(func), Exception(""), IsExceptionThrown(false)
            { }

            ThreadStartMethod Func;

            Exception Exception;
            bool IsExceptionThrown;
        };

        ThreadContext ctx;

        Thread(ThreadStartMethod method) :
            ctx(method)
        { }

        void Start() override
        {
            StartThread((thdStartMethod)RawStart, (void*)&this->ctx);
        }

        static void* RawStart(ThreadContext* ctx)
        {
            try
            {
                ctx->Func();
            }
            catch (Exception& ex)
            {
                ctx->Exception = ex;
                ctx->IsExceptionThrown = true;
                NotifyErrorHandlers(ex);
            }

            return null;
        }
    };


    template<typename TArg>
    class Thread<TArg> : public ThreadBase
    {
    public:
        using ThreadStartMethod = void(*)(TArg arg);

        void Join() override
        {
            JoinThread();

            if (ctx.IsExceptionThrown)
                throw ctx.Exception;
        }

        ~Thread()
        {
            Join();
        }

        static Thread<TArg>* Run(ThreadStartMethod method, TArg arg)
        {
            var thread = new Thread<TArg>(method, arg);
            thread->Start();
            return thread;
        }

    private:
        struct ThreadContext
        {
            ThreadContext(ThreadStartMethod func, TArg arg)
                :Func(func), Arg(arg), Exception(""), IsExceptionThrown(false)
            { }

            ThreadStartMethod Func;
            TArg Arg;

            Exception Exception;
            bool IsExceptionThrown;
        };

        ThreadContext ctx;

        Thread(ThreadStartMethod methodArg1Res, TArg arg) :
            ctx(methodArg1Res, arg)
        {
        }

        void Start() override
        {
            StartThread((thdStartMethod)RawStart, (void*)&this->ctx);
        }

        static void* RawStart(ThreadContext* ctx)
        {
            try
            {
                ctx->Func(ctx->Arg);
            }
            catch (Exception& ex)
            {
                ctx->Exception = ex;
                ctx->IsExceptionThrown = true;
                NotifyErrorHandlers(ex);
            }

            return null;
        }
    };


    template<typename TArg, typename TResult>
    class Thread<TArg, TResult> : public ThreadBase
    {
    public:
        using ThreadStartMethod = TResult(*)(TArg arg);

        void Join() override
        {
            JoinThread();

            if (ctx.IsExceptionThrown)
                throw ctx.Exception;
        }

        TResult Result()
        {
            Join();
            return this->ctx.Result;
        };

        ~Thread()
        {
            Join();
        }

        static Thread<TArg, TResult>* Run(ThreadStartMethod method, TArg arg)
        {
            var thread = new Thread<TArg, TResult>(method, arg);
            thread->Start();
            return thread;
        }

    private:
        struct ThreadContext
        {
            ThreadContext(ThreadStartMethod func, TArg arg)
                :Func(func), Arg(arg), Exception(""), IsExceptionThrown(false)
            { }

            ThreadStartMethod Func;
            TArg Arg;
            TResult Result;

            Exception Exception;
            bool IsExceptionThrown;
        };

        ThreadContext ctx;

        Thread(ThreadStartMethod methodArg1Res, TArg arg) :
            ctx(methodArg1Res, arg)
        { 
        }

        void Start() override
        {
            StartThread((thdStartMethod)RawStart, (void*)&this->ctx);
        }

        static void* RawStart(ThreadContext* ctx)
        {
            try
            {
                ctx->Result = ctx->Func(ctx->Arg);
            }
            catch (Exception& ex)
            {
                ctx->Exception = ex;
                ctx->IsExceptionThrown = true;
                NotifyErrorHandlers(ex);
            }

            return null;
        }
    };

}