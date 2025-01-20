#pragma once
#include "Definitions.h"
#include "Exception.h"

#define TTC   template<typename T1, typename T2>
#define TTTC  template<typename T1, typename T2, typename T3>
#define TTTTC  template<typename T1, typename T2, typename T3, typename T4>
#define TTTTTC  template<typename T1, typename T2, typename T3, typename T4, typename T5>

//the idea is taken from: https://stackoverflow.com/questions/9469418/class-templates-with-duplicate-names
namespace System
{
	template<typename...> struct Tuple;

	TTC struct Tuple<T1, T2>
	{
	public:
		Tuple(T1 item1, T2 item2) :
			Item1(item1), Item2(item2)
		{ }

		T1 Item1;
		T2 Item2;

		operator string()
		{
			return (string)"(" + this->Item1 + ", " + this->Item2 + ")";
		}

		friend bool operator == (const Tuple<T1, T2>& tA, const Tuple<T1, T2>& tB)
		{
			return tA.Item1 == tB.Item1 &&
				tA.Item2 == tB.Item2;
		}

		friend bool operator != (const Tuple<T1, T2>& tA, const Tuple<T1, T2>& tB)
		{
			return !(tA == tB);
		}
	};

	TTTC struct Tuple<T1, T2, T3>
	{
	public:
		Tuple(T1 item1, T2 item2, T3 item3) :
			Item1(item1), Item2(item2), Item3(item3)
		{ }

		T1 Item1;
		T2 Item2;
		T3 Item3;

		operator string()
		{
			return (string)"(" + this->Item1 + ", " + this->Item2 + ", " + this->Item3 + ")";
		}

		friend bool operator == (const Tuple<T1, T2, T3>& tA, const Tuple<T1, T2, T3>& tB)
		{
			return tA.Item1 == tB.Item1 &&
				tA.Item2 == tB.Item2 &&
				tA.Item3 == tB.Item3;
		}

		friend bool operator != (const Tuple<T1, T2, T3>& tA, const Tuple<T1, T2, T3>& tB)
		{
			return !(tA == tB);
		}
	};

	TTTTC struct Tuple<T1, T2, T3, T4>
	{
	public:
		Tuple(T1 item1, T2 item2, T3 item3, T4 item4) :
			Item1(item1), Item2(item2), Item3(item3), Item4(item4)
		{ }

	    T1 Item1;
		T2 Item2;
		T3 Item3;
		T4 Item4;

		operator string()
		{
			return (string)"(" + this->Item1 + ", " + this->Item2 + ", " + this->Item3 + ", " + this->Item4 + ")";
		}

		friend bool operator == (const Tuple<T1, T2, T3, T4>& tA, const Tuple<T1, T2, T3, T4>& tB)
		{
			return tA.Item1 == tB.Item1 &&
				tA.Item2 == tB.Item2 &&
				tA.Item3 == tB.Item3 &&
				tA.Item4 == tB.Item4;
		}

		friend bool operator != (const Tuple<T1, T2, T3, T4>& tA, const Tuple<T1, T2, T3, T4>& tB)
		{
			return !(tA == tB);
		}
	};

	TTTTTC struct Tuple<T1, T2, T3, T4, T5>
	{
	public:
		Tuple(T1 item1, T2 item2, T3 item3, T4 item4, T5 item5) :
			Item1(item1), Item2(item2), Item3(item3), Item4(item4), Item5(item5)
		{ }

	    T1 Item1;
		T2 Item2;
		T3 Item3;
		T4 Item4;
		T5 Item5;

		operator string()
		{
			return (string)"(" + this->Item1 + ", " + this->Item2 + ", " + this->Item3 + ", " + this->Item4 + + this->Item5 + ")";
		}

		friend bool operator == (const Tuple<T1, T2, T3, T4, T5>& tA, const Tuple<T1, T2, T3, T4, T5>& tB)
		{
			return tA.Item1 == tB.Item1 &&
				tA.Item2 == tB.Item2 &&
				tA.Item3 == tB.Item3 &&
				tA.Item4 == tB.Item4 &&
				tA.Item5 == tB.Item5;
		}

		friend bool operator != (const Tuple<T1, T2, T3, T4, T5>& tA, const Tuple<T1, T2, T3, T4, T5>& tB)
		{
			return !(tA == tB);
		}
	};
}