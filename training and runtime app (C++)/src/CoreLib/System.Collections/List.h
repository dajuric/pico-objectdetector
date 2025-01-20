#pragma once
#include "System.h"

namespace System::Collections::Generic
{
    TC class List: public Enumerable<T*, T>, public Collection<T>
    {
    public:
        List()
        {
            this->data = new T[INITIAL_CAPACITY];
            this->length = 0;
            this->capacity = INITIAL_CAPACITY;
        }

        List(InitializerList<T> initList)
            :List()
        {
            for (var elem : initList)
                Add(elem);
        }

        List(const T* ptr, long length)
            :List()
        {
            for (var i = 0; i < length; i++)
                Add(ptr[i]);
        }

        List(const List& source)
            :List()
        {
            *this = source; //call the assign operator
        }

        ~List()
        {
            Clear();
            delete[] this->data;
            this->capacity = 0;
        }

        List<T>& operator=(const List<T>& other)
        {
            Clear();

            for (var i = 0; i < other.length; i++)
                Add(other.data[i]);

            return *this;
        }

        T& operator [](int index)
        {
            if (index < 0 || index >= this->length)
                throw IndexOutOfRangeException("Can not get element. Index out of range.");

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


        long Count()
        {
            return this->length;
        }

        long Capacity()
        {
            return this->capacity;
        }

        bool IsEmpty()
        {
            return this->length == 0;
        }

        bool Contains(const T& item)
        {
            for (var i = 0; i < this->length; i++)
            {
                var isHit = this->data[i] == item;
                if (isHit) return true;
            }

            return false;
        }

        void Add(const T& item)
        {
            EnsureCapacity();

            this->data[(int)this->length] = item;
            this->length++;
        }

        void AddRange(Collection<T>& storage)
        {
            for (var i = 0; i < storage.Count(); i++)
                Add(storage[i]);
        }

        void Insert(int index, const T& item)
        {
            if (index < 0 || index > this->length)
                throw IndexOutOfRangeException("Can not insert element. Index out of range.");

            EnsureCapacity();

            for (var i = this->length; i >= index; i--)
                this->data[i + 1] = this->data[i];

            this->data[index] = item;
            this->length++;
        }

        void RemoveAt(int index)
        {
            if (index < 0 || index >= this->length)
                throw IndexOutOfRangeException("Can not remove element. Index out of range.");

            for (var i = index; i < this->length; i++)
                this->data[i] = this->data[i + 1];

            this->length--;
        }

        bool Remove(const T& item)
        {
            var idx = Collection<T>::FindIndex(item);
            if (idx < 0) return false;

            RemoveAt(idx);
            return true;
        }

        void RemoveLast()
        {
            if (this->length == 0)
                return;

            this->length--;
        }

        void Clear()
        {
            this->length = 0;
        }

        void ToNativeArray(T* nativeArray)
        {
            if (nativeArray == null)
                throw ArgumentException("The native array argument is null.");

            for (var i = 0; i < this->length; i++)
                nativeArray[i] = this->data[i];
        }

        List<T> GetAt(Collection<int>& indices)
        {
           List<T> result;
           for (var i = 0; i < indices.Count(); i++)
            {
                var idx = indices[i];
                if (idx < 0 || idx >= this->length)
                    throw IndexOutOfRangeException("One of the provided indices is out of range.");

                var elem = this->data[idx];
                result.Add(elem); 
            }

            return result;
        }

        List<T> Get(Collection<bool>& mask)
        {
            if (mask.Count() != this->Count())
                throw Exception("Mask must contain the same number of elements as the source collection.");

            List<T> result;
            for (var i = 0; i < mask.Count(); i++)
            {
                if (mask[i] == false)
                    continue;

                result.Add(this->data[i]); 
            }

            return result;
        }

        void Add(const T& item, int count)
        {
            for (var i = 0; i < count; i++)
                Add(item);
        }

    private:
        const int INITIAL_CAPACITY = 4;

        T* data;
        long length;
        long capacity;

        void EnsureCapacity()
        {
            if (this->length < this->capacity)
                return;

            var oldData = this->data;
            this->capacity *= 2;
            this->data = new T[this->capacity];

            for (var i = 0; i < this->length; i++)
                this->data[i] = oldData[i];

            delete[] oldData;
        }
    };
}
