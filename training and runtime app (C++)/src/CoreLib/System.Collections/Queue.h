#pragma once
#include "System.h"
#include "LinkedList.h"

namespace System::Collections::Generic
{
	TC class QueueIterator
	{
	public:
		QueueIterator(LinkedListNode<T>* lastNode)
		{
			this->node = lastNode;
		}

		QueueIterator<T>& operator++()
		{
			if (this->node != null)
				this->node = this->node->Prev;

			return *this;
		}

		QueueIterator<T>& operator++(int)
		{
			var iterator = *this;
			++* this;
			return iterator;
		}

		bool operator != (const QueueIterator<T>& iterator)
		{
			return this->node != iterator.node;
		}

		T operator *()
		{
			return this->node->Data;
		}

	private:
		LinkedListNode<T>* node;
	};

	TC class Queue: public Enumerable<QueueIterator<T>, T>
	{
	public:
		Queue()
		{
			this->list = new LinkedList<T>();
		}

		Queue(InitializerList<T> initList)
			:Queue()
		{
			for (var elem : initList)
				this->list->AddFirst(elem);
		}

		Queue(const Queue<T>& source)
			:Queue()
		{
			*this = source;
		}

		~Queue()
		{
			delete this->list;
		}

		Queue& operator =(const Queue& other)
		{
			this->list->Clear();

			for (var elem : other->list)
				this->list->AddFirst(elem);
		}

		QueueIterator<T> begin()
		{
			return QueueIterator<T>(this->list->Last());
		}

		QueueIterator<T> end()
		{
			return QueueIterator<T>(null);
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
				throw InvalidOperationException("Queue empty");

			return this->list->Last()->Data;
		}

		T Dequeue()
		{
			if (this->list->IsEmpty())
				throw InvalidOperationException("Queue empty");

			var data = list->Last()->Data;
			this->list->RemoveLast();

			return data;
		}

		void Enqueue(const T& item)
		{
			this->list->AddFirst(item);
		}

	private:
		LinkedList<T>* list;
	};
}