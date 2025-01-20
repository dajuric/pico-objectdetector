#pragma once
#include "System.h"
#include "Stream.h"
#include <stdio.h>
#include <memory.h>

using namespace System;

namespace System::IO
{
	class MemoryStream: public Stream
	{
	public:
		MemoryStream(int capacity = 128 * 1024)
			:Stream()
		{
			this->data = new byte[capacity];
			this->capacity = capacity;
			this->isAllocated = true;
		}

		MemoryStream(byte* data, long length)
			:Stream()
		{
			this->data = data;
			this->length = length;
			this->capacity = length;
			this->isAllocated = false;
		}

		~MemoryStream()
		{
			Close();
		}

		MemoryStream(const MemoryStream& other)
			:Stream()
		{
			this->data = new byte[other.capacity];
			this->capacity = other.capacity;
			this->length = other.length;
			this->isAllocated = true;

			memcpy(this->data, other.data, this->length);
		}

		MemoryStream& operator = (const MemoryStream&) = delete;


		void Close() override
		{
			if (this->data == null)
				return;

			if (this->isAllocated) 
				delete[] this->data;

			this->data = null;
			this->capacity = 0;

			Stream::Close();
		}

		long Read(byte* buffer, int count) override
		{		
			count = (int)Math::Min((long)count, this->length - this->position);
			memcpy(buffer, &this->data[this->position], count);

			this->position += count;
			return count;
		}

		void Write(byte* buffer, int count) override
		{	
			if (this->isAllocated == false)
				throw Exception("Stream is not allocated. Writing is not permitted.");

			EnsureCapacity(this->position + count);
			memcpy(&this->data[this->position], buffer, count);

			this->position += count;
			this->length = Math::Max(this->position, this->length);
		}

		void Seek(long position) override
		{
			if (position < 0 || position > this->length)
				throw ArgumentException("Invalid position.");

			this->position = position;
		}

		void ToFile(const string& filePath)
		{
			FILE* f = fopen(filePath.Ptr(), "wb");
			if (f == null)
				throw IOException((string)"File can not be opened for writing. " + " (" + filePath + ")");

			fwrite(this->data, 1, this->length, f);
			fflush(f);
			fclose(f);
		}

		byte* Ptr(long pos = 0)
		{
			if (pos < 0 || pos > this->length)
				throw ArgumentException("Invalid position.");

			return &this->data[pos];
		}

	private:
		byte* data;
		long capacity;
		bool isAllocated;

		void EnsureCapacity(long requiredCapacity)
		{
			if (requiredCapacity <= this->capacity)
				return;

			var oldData = this->data;
			this->capacity = Math::Max(requiredCapacity, this->capacity * 2);
			this->data = new byte[this->capacity];

			memcpy(this->data, oldData, this->length);
			delete[] oldData;
		}
	};
}
