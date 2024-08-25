#include "Definitions.h"

#define TEl template<typename TElem>

namespace System
{
    TEl abstract class Collection
    {
    public:
        virtual TElem& operator [](int index) = 0;

        virtual long Count() = 0;

        bool IsEmpty()
        {
            return Count() == 0;
        }

        int FindIndex(const TElem& item)
        {
            var& arr = *this;

            for (var i = 0; i < this->Count(); i++)
            {
                var isHit = (arr[i] == item);
                if (isHit) return i;
            }

            return -1;
        }

        bool Contains(const TElem& item)
        {
            return FindIndex(item) != -1;
        }

        //---------------- Quick Sort
        using IsLessThan = bool (*)(TElem& elemA, TElem& elemB);

        void Sort(IsLessThan isLessThan)
        {
            QuickSort(0, this->Count() - 1, isLessThan);
        }

        void Sort()
        {
            QuickSort(0, this->Count() - 1, [](TElem& a, TElem& b) { return a < b; });
        }

    private:
        //---------------- Quick Sort
        static void Swap(TElem* a, TElem* b)
        {
            TElem t = *a;
            *a = *b;
            *b = t;
        }

        int Partition(int low, int high, IsLessThan isLessThan)
        {
            var& arr = *this;
            var& pivot = arr[high];
            int i = (low - 1);

            for (int j = low; j <= high - 1; j++)
            {
                var& e = arr[j];
                if (isLessThan(e, pivot))
                {
                    i++;
                    Swap(&arr[i], &arr[j]);
                }
            }

            Swap(&arr[i + 1], &arr[high]);
            return (i + 1);
        }

        void QuickSort(int low, int high, IsLessThan isLessThan)
        {
            if (low < high)
            {
                int pi = Partition(low, high, isLessThan);

                QuickSort(low, pi - 1, isLessThan);
                QuickSort(pi + 1, high, isLessThan);
            }
        }
    };
}
