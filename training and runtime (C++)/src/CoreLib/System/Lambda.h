#pragma once
#include "Definitions.h"

//the idea is taken from: https://stackoverflow.com/questions/7852101/c-lambda-with-captures-as-a-function-pointer
class Lambda
{
public:
    template<typename TRet = void, typename TFp = TRet(*)(), typename F>
    static TFp Ptr0(F& f)
    {
        fn<F>(&f);
        return (TFp)execFunc<TRet, F>;
    }

    template<typename T, typename TRet = void, typename TFp = TRet(*)(T&), typename F>
    static TFp Ptr1(F& f)
    {
        fn<F>(&f);
        return (TFp)execFunc<T, TRet, F>;
    }

    template<typename T1, typename T2, typename TRet = void, typename TFp = TRet(*)(T1&, T2&), typename F>
    static TFp Ptr2(F& f)
    {
        fn<F>(&f);
        return (TFp)execFunc<T1, T2, TRet, F>;
    }

private:
    template<typename F>
    static void* fn(void* newFn = null)
    {
        static void* fn;
        if (newFn != null)
            fn = newFn;

        return fn;
    }

    template<typename TRet, typename F>
    static TRet execFunc()
    {
        return (TRet)(*(F*)fn<F>())();
    }

    template<typename T, typename TRet, typename F>
    static TRet execFunc(T& arg)
    {
        return (TRet)(*(F*)fn<F>())(arg);
    }

    template<typename T1, typename T2, typename TRet, typename F>
    static TRet execFunc(T1& arg1, T2& arg2)
    {
        return (TRet)(*(F*)fn<F>())(arg1, arg2);
    }
};
