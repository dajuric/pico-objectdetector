#pragma once
#include "System.h"

namespace System::Collections::Generic
{
    TC class Array : public Enumerable<T*, T>, public Collection<T>
    {
    public:
        Array(int length)
        {
            if (length <= 0)
                throw ArgumentException("Can not create an empty array."); //Argument out of range

            this->data = new T[length];
            this->length = length;
        }

        Array(InitializerList<T> initList)
        {
            this->length = 0;
            for (var elem : initList) this->length++;
            this->data = new T[length];

            var i = 0;
            for (var elem : initList)
            {
                this->data[i] = elem;
                i++;
            }
        }

        Array(const T* ptr, long length)
            :Array(length)
        {
            for (var i = 0; i < length; i++)
                this->data[i] = ptr[i];
        }

        Array(const Array& source)
            :Array(source.length)
        {
            *this = source; //call the assign operator
        }

        ~Array()
        {
            delete[] this->data;
            this->length = 0;
        }

        Array<T>& operator=(const Array<T>& other)
        {
            for (var i = 0; i < other.length; i++)
                this->data[i] = other.data[i];

            this->length = other.length;
            return *this;
        }

        T& operator [](int index)
        {
            if (index < 0 || index >= this->length)
                throw IndexOutOfRangeException("Index out of range.");

            return this->data[index];
        }

        T* begin()
        {
            return this->data;
        }

        T* end()
        {
            return this->data + this->length;
        }

        long Length()
        {
            return this->length;
        }

        long Count() //collection interface
        {
            return this->length;
        }

        T* Ptr(int position = 0)
        {
            if (position >= length)
                throw Exception("Position exceeds the array length.");

            return &data[position];
        }

    private:
        T* data;
        long length;
    };
}
