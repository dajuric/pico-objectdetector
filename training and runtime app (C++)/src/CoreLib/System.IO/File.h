#pragma once
#include "System.h"
#include "System.Collections.h"
#include "FileStream.h"
#include <sys/stat.h>

using namespace System;
using namespace System::Collections::Generic;

namespace System::IO
{
	class File
	{
	public:
		static List<string> ReadAllLines(string path)
		{
			var fs = FileStream(path, FileMode::ReadOnly);

			List<string> lines;
			while (fs.IsEOF() == false)
			{
				var line = fs.ReadLine();
				lines.Add(line);
			}

			fs.Close();
			return lines;
		}

		static void WriteAllLines(string path, List<string> lines)
		{
			var fs = FileStream(path, FileMode::WriteOnly);

			for (var line : lines)
			{
				fs.WriteLine(line);
			}

			fs.Close();
		}

		static MemoryStream* ReadAllContent(string path)
		{
			var fs = FileStream(path, FileMode::ReadOnly);
			var ms = new MemoryStream();

			const int buffLen = 4 * 1024;
			byte buff[buffLen] = { 0 };

			while (fs.IsEOF() == false)
			{
				int readCount = fs.Read(buff, buffLen);
				ms->Write(buff, readCount);
			}

			fs.Close();
			ms->Seek(0);

			return ms;
		}

		static bool Exists(string path)
		{
			struct stat buffer;
			var exists = stat(path.Ptr(), &buffer) == 0;
			return exists;
		}

	};
}
