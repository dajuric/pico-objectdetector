#pragma once
#include "System.h"

namespace System::Collections::Generic
{
    TC class LinkedListNode
    {
    public:
        LinkedListNode<T>* Prev;
        LinkedListNode<T>* Next;  
        T Data;

        LinkedListNode() :
            Prev(null), Next(null)
        { }

        LinkedListNode(const T& data) :
            Prev(null), Next(null), Data(data)
        { }

        ~LinkedListNode()
        {
            this->Prev = null;
            this->Next = null;
        }
    };

    TC class LinkedListIterator
    {
    public:
        LinkedListIterator(LinkedListNode<T>* startNode)
        {
            this->node = startNode;
        }

        LinkedListIterator<T>& operator++()
        {
            if (this->node != null)
                this->node = this->node->Next;

            return *this;
        }

        LinkedListIterator<T>& operator++(int)
        {
            var iterator = *this;
            ++* this;
            return iterator;
        }

        bool operator != (const LinkedListIterator<T>& iterator)
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

    TC class LinkedList: public Enumerable<LinkedListIterator<T>, T>, public Collection<T>
    {
    public:
        LinkedList()
        {
            this->head = null;
            this->tail = null;
            this->count = 0;
        }
    
        LinkedList(InitializerList<T> initList)
        {
            for (var elem: initList)
                AddLast(elem);
        }

        LinkedList(const LinkedList& source)
        {
            *this = source; //call the assign operator
        }

        ~LinkedList()
        {
            Clear();
        }

        LinkedList<T>& operator=(const LinkedList<T>& other)
        {
            Clear();

            var curr = other.head;
            while (curr != null)
            {
                AddLast(curr->Data);
                curr = curr->Next;
            }

            return *this;
        }

        T& operator [](int index)
        {
            if (index < 0 || index >= this->count)
                throw IndexOutOfRangeException("Index out of range.");

            LinkedListNode<T>* curr = this->head;
            while (index > 0)
            {
                curr = curr->Next;
                index--;
            }

            return curr->Data;
        }

        LinkedListIterator<T> begin()
        {
            return LinkedListIterator<T>(this->head);
        }

        LinkedListIterator<T> end()
        {
            return LinkedListIterator<T>(null);
        }


        long Count()
        {
            return this->count;
        }

        bool IsEmpty()
        {
            return this->count == 0;
        }

        void AddFirst(const T& item)
        {
            var newNode = new LinkedListNode<T>(item);

            if (this->head == null)
            {
                this->tail = newNode;
            }
            else
            {
                var oldHead = this->head;
                oldHead->Prev = newNode;
                newNode->Next = oldHead;
            }

            this->head = newNode;
            this->count++;
        }

        void AddLast(const T& item)
        {
            var newNode = new LinkedListNode<T>(item);

            if (this->head == null)
            {
                this->head = newNode;
            }
            else
            {
                var oldTail = this->tail;
                oldTail->Next = newNode;
                newNode->Prev = oldTail;              
            }

            this->tail = newNode;
            this->count++;
        }

        void RemoveFirst()
        {
            if (this->head == null)
                return;

            var oldHead = this->head;
            this->head = oldHead->Next;

            if (this->head == null)
                this->tail = null;
            else         
                this->head->Prev = null;

            delete oldHead;
            this->count--;
        }

        void RemoveLast()
        {
            if (this->head == null)
                return;

            var oldTail = this->tail;
            this->tail = oldTail->Prev;

            if (this->tail == null)
                this->head = null;
            else
                this->tail->Next = null;

            delete oldTail;
            this->count--;
        }

        const LinkedListNode<T>* Find(const T& item)
        {
            LinkedListNode<T>* curr = this->head;
            while (curr != null && curr->Data != item)
                curr = curr->Next;

            return curr;
        }

        bool Contains(const T& item)
        {
            return Find(item) != null;
        }

        bool Remove(const T& item)
        {
            if (item == this->head->Data)
            {
                RemoveFirst();
                return true;
            }

            if (item == this->tail->Data)
            {
                RemoveLast();
                return true;
            }

            var nodeToRemove = Find(item);
            if (nodeToRemove != null)
            {
                var prevNode = nodeToRemove->Prev;
                var nextNode = nodeToRemove->Next;

                prevNode->Next = nextNode;
                nextNode->Prev = prevNode;

                delete nodeToRemove;
                this->count--;

                return true;
            }

            return false;
        }

        void Reverse()
        {
            LinkedListNode<T>* curr = this->head;
            LinkedListNode<T>* prev = null, * next = null;

            while (curr != null)
            {
                next = curr->Next;
                curr->Next = prev;

                prev = curr;
                curr = next;
            }

            this->head = prev;
        }

        void Clear()
        {
            var curr = this->head;

            while (curr != null)
            {
                var next = curr->Next;
                delete curr;
                curr = next;
            }

            this->head = null;
            this->tail = null;
            this->count = 0;
        }

        LinkedListNode<T>* First() 
        { 
            return this->head; 
        }

        LinkedListNode<T>* Last()
        {
            return this->tail;
        }

    private:
        LinkedListNode<T>* head;
        LinkedListNode<T>* tail;
        long count;
    };
}