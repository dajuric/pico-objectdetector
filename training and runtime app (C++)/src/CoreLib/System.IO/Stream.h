#pragma once
#include "System.h"

using namespace System;

namespace System::IO
{
	abstract class Stream
	{
	public:
		~Stream()
		{
			Close();
		}

		Stream(const Stream& other) = delete;

		Stream& operator = (const Stream&) = delete;


		virtual void Close()
		{
			this->position = -1;
			this->length = 0;
		}

		long Position()
		{
			return this->position;
		}

		long Length()
		{
			return this->length;
		}

		bool IsEOF()
		{
			return this->position >= this->length;
		}

		virtual long Read(byte* buffer, int count) = 0;

		virtual void Write(byte* buffer, int count) = 0;

		virtual void Seek(long position) = 0;


		template <typename T>
		T ReadValue()
		{
			T value;
			Read((byte*)&value, sizeof(T));
			return value;
		}

		template <typename T>
		void WriteValue(T value)
		{
			Write((byte*)&value, sizeof(T));
		}


		string ReadString()
		{
			return ReadString('\0');
		}

		string ReadLine()
		{
			return ReadString('\n');
		}

		void WriteLine(const string& line)
		{
			Write((byte*)line.Ptr(), line.Length());
			Write((byte*)"\n", 1);
		}

	protected:
		long position;
		long length;

		Stream()
		{
			this->position = 0;
			this->length = 0;
		}

	private:
		string ReadString(char stopChar)
		{
			string readStr = "";
			const int READ_BUFFER = 1024;
			byte buff[READ_BUFFER] = { 0 };

			while (IsEOF() == false)
			{
				var readCount = Read(buff, READ_BUFFER);
				var stopCharIdx = FindCharOrNull((char*)buff, readCount, stopChar);

				if (stopCharIdx >= 0) buff[stopCharIdx] = '\0';
				readStr = readStr + (char*)buff;

				if (stopCharIdx >= 0)
				{
					Seek(this->position - readCount + stopCharIdx + 1);
					break;
				}
			}

			return readStr;
		}

		static int FindCharOrNull(char* str, int strLen, char ch)
		{
			for (var i = 0; i < strLen; i++)
			{
				if (str[i] == ch || str[i] == '\0')
					return i;
			}

			return -1;
		}
	};
}
