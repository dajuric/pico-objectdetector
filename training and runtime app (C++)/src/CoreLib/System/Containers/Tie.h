#pragma once
#include "Definitions.h"
#include "Tuple.h"

#define TTC   template<typename T1, typename T2>
#define TTTC  template<typename T1, typename T2, typename T3>
#define TTTTC  template<typename T1, typename T2, typename T3, typename T4>

namespace System
{
    template<typename...> struct Tie;

    TTC struct Tie<T1, T2>
    {
    public:
        Tie(T1& item1, T2& item2):
            item1(item1), item2(item2)
        { }

        auto operator=(const Tuple<T1, T2>& tuple)
        {
            this->item1 = tuple.Item1;
            this->item2 = tuple.Item2;
        }
    
    private:
        T1& item1;
        T2& item2;
    };


    TTTC struct Tie<T1, T2, T3>
    {
    public:
        Tie(T1& item1, T2& item2, T3& item3):
            item1(item1), item2(item2), item3(item3)
        { }

        auto operator=(const Tuple<T1, T2, T3>& tuple)
        {
            this->item1 = tuple.Item1;
            this->item2 = tuple.Item2;
            this->item3 = tuple.Item3;
        }
    
    private:
        T1& item1;
        T2& item2;
        T3& item3;
    };


    TTTTC struct Tie<T1, T2, T3, T4>
    {
    public:
        Tie(T1& item1, T2& item2, T3& item3, T4& item4):
            item1(item1), item2(item2), item3(item3), item4(item4)
        { }

        auto operator=(const Tuple<T1, T2, T3, T4>& tuple)
        {
            this->item1 = tuple.Item1;
            this->item2 = tuple.Item2;
            this->item3 = tuple.Item3;
            this->item4 = tuple.Item4;
        }
    
    private:
        T1& item1;
        T2& item2;
        T3& item3;
        T4& item4;
    };
	
}