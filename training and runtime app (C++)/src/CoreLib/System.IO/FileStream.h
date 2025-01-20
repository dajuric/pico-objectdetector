#pragma once
#include "System.h"
#include "Stream.h"

#include <stdio.h>

using namespace System;

namespace System::IO
{
	enum class FileMode
	{
		ReadOnly = 0,
		WriteOnly = 1,
		ReadWrite = 2
	};

	class FileStream: public Stream
	{
	public:
		FileStream(const string& path, FileMode fileMode)
			:FileStream()
		{
			switch (fileMode)
			{
			case FileMode::ReadOnly:
				this->filePtr = fopen(path.Ptr(), "rb");
				break;
			case FileMode::WriteOnly:
				this->filePtr = fopen(path.Ptr(), "wb");
				break;
			case FileMode::ReadWrite:
				this->filePtr = fopen(path.Ptr(), "wb+");
				break;
			}

			if (this->filePtr == null)
			{
				throw IOException("Can not open the specifed file: " + path);
			}

			if (fileMode != FileMode::WriteOnly)
			{
				fseek(this->filePtr, 0, SEEK_END);
				this->length = ftell(this->filePtr);
				fseek(this->filePtr, 0, SEEK_SET);
			}

			this->mode = fileMode;
		}

		~FileStream()
		{
			Close();
		}

		FileStream(const FileStream& other) = delete;

		FileStream& operator = (const FileStream&) = delete;


		void Close() override
		{
			if (this->filePtr == null)
				return;

			fclose(this->filePtr);
			this->filePtr = null;
			Stream::Close();
		}

		FileMode OpenMode()
		{
			return this->mode;
		}

		long Read(byte* buffer, int count) override
		{
			if (this->mode == FileMode::WriteOnly)
				throw InvalidOperationException("Stream is write-only");

			var readCount = (long)fread(buffer, sizeof(byte), count, this->filePtr);
			this->position += readCount;

			return readCount;
		}

		void Write(byte* buffer, int count) override
		{
			if (this->mode == FileMode::ReadOnly)
				throw InvalidOperationException("Stream is read-only");

			var writtenCount = fwrite(buffer, sizeof(byte), count, this->filePtr);
			if (writtenCount != count)
				throw IOException("Error writing file.");

			this->position += count;
			this->length = Math::Max(this->position, this->length);
		}

		void Seek(long position) override
		{
			bool isOK = fseek(this->filePtr, position, SEEK_SET) == 0;
			if (!isOK)
				throw Exception((string)"Can not seek to the requested position: " + position);

			this->position = position;
		}

	private:
		FILE* filePtr;
		FileMode mode;

		FileStream()
			:Stream()
		{
			this->filePtr = null;
			this->length = -1;
			this->mode = FileMode::ReadOnly;
		}
	};
}
