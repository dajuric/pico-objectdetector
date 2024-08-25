#pragma once
#include "System.h"
#include "System.Collections.h"
#include "StringExtensions.h"

using namespace System;
using namespace System::Collections::Generic;

namespace System::IO
{
	class Path
	{
	public:
		static string Combine(const string& pathA, const string& pathB)
		{
			List<string> paths = { pathA, pathB };
			var path = Combine(paths);

			return path;
		}

		static string Combine(Collection<string>& paths)
		{
			List<char> rawPath;

			for (var i = 0; i < paths.Count(); i++)
			{
				rawPath<-AddString(paths[i]);

				if (rawPath.Count() != 0 && i < (paths.Count() - 1)) //do not add '/' at the end and at the beginning if not intended
					rawPath.Add('/');
			}
	
			var path = NormalizePath(rawPath);
			return path;
		}

		static Tuple<string, string, string> FileParts(const string& path)
		{
			// path
			var fNameIdx = path.Find('/', 0, true);
			if (fNameIdx == -1)
				fNameIdx = path.Find('\\', 0, true);

			if (fNameIdx == -1) //no path
				fNameIdx = 0;

			fNameIdx++;

			//fName
			var extIdx = path.Find('.', 0, true);
			if (extIdx == -1) //no ext
				extIdx = path.Length() - 1;

			
			var fPath = (fNameIdx > 0) ? path.Slice(0, fNameIdx - 1): String();
			var fName = (fNameIdx < (path.Length() - 1)) ? path.Slice(fNameIdx, extIdx - 1): String();
			var fExt  = (extIdx < (path.Length() - 1)) ? path.Slice(extIdx): String();

			return Tuple<string, string, string>(fPath, fName, fExt);
		}

	private:
		static string NormalizePath(List<char> rawPath)
		{
			List<char> path;
			bool wasDelimiter = false;

			for (var i = 0; i < rawPath.Count(); i++)
			{
				//if not delimiter add char and continue
				if (rawPath[i] != '\\' && rawPath[i] != '/')
				{
					path.Add(rawPath[i]);
					wasDelimiter = false;
					continue;
				}

				//if delimiter add it once
				if (wasDelimiter == false)
				{
					path.Add('/');
					wasDelimiter = true;
				}
			}

			return path<-ToString();
		}
	};
}