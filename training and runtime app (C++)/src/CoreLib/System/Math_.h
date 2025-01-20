#pragma once
#include "Definitions.h"

#include <math.h>

namespace System
{
    class Math
    {
    public:
        inline static double Exp(double x)
        {
            return exp(x);
        }

        inline static double Pow(double base, double exp)
        {
            return pow(base, exp);
        }

        inline static double Log2(double x)
        {
            return log2(x);
        }

        inline static double Log(double x)
        {
            return log(x);
        }

        inline static double Sqrt(double x)
        {
            return sqrt(x);
        }

        template<typename T>
        inline static T Min(T a, T b)
        {
            return (a < b) ? a : b;
        }

        template<typename T>
        inline static T Max(T a, T b)
        {
            return (a > b) ? a : b;
        }

        inline static double Ceil(double x)
        {
            return ceil(x);
        }

        inline static double Floor(double x)
        {
            return floor(x);
        }

        inline static double Round(double x, int decimals = 0)
        {
            double value = (int)(x * Math::Pow(10, decimals) + 0.5);
            value /= Math::Pow(10, decimals);

            return value;
        }

        template<typename T>
        inline static T Abs(T x)
        {
            if (x < 0) return -x;
            return x;
        }
    };
}