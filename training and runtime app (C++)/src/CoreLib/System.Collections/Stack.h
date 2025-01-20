#pragma once
#include "System.h"
#include "LinkedList.h"

namespace System::Collections::Generic
{
	TC using StackIterator = LinkedListIterator<T>;

	TC class Stack: public Enumerable<StackIterator<T>, T>
	{
	public:
		Stack()
		{
			this->list = new LinkedList<T>();
		}

		Stack(InitializerList<T> initList)
			:Stack()
		{
			for (var elem : initList)
				this->list->AddFirst(elem);
		}

		Stack(const Stack<T>& source)
			:Stack()
		{
			*this = source;
		}

		~Stack()
		{
			delete this->list;
		}

		Stack& operator =(const Stack& other)
		{
			this->list->Clear();

			for (var elem : other->list)
				this->list->AddFirst(elem);
		}

		StackIterator<T> begin()
		{
			return StackIterator<T>(this->list->First());
		}

		StackIterator<T> end()
		{
			return StackIterator<T>(null);
		}
		
		long Count()
		{
			return this->list->Count();
		}

		void Clear()
		{
			this->list->Clear();
		}

		bool Contains(const T& item)
		{
			return this->list->Contains(item);
		}

		const T& Peek()
		{
			if (this->list->IsEmpty())
				throw InvalidOperationException("Stack empty");

			return this->list->First()->Data;
		}

		const T& Pop()
		{
			if (this->list->IsEmpty())
				throw InvalidOperationException("Stack empty");

			var first = list->First();
			this->list->RemoveFirst();

			return first->Data;
		}

		void Push(const T& item)
		{
			this->list->AddFirst(item);
		}

	private:
		LinkedList<T>* list;
	};
}
