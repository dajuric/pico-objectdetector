#pragma once
#include "System.h"
#include "System.Collections.h"
#include <string.h>
#include <sys/stat.h>
#ifdef _WIN32
#include "3rdParty/win_dirent.h"
#include <direct.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <dirent.h>
#endif

using namespace System;
using namespace System::Collections::Generic;

namespace System::IO
{
	class Directory
	{
	public:
		static List<string> GetFiles(const string& dirPath, const string& fileExt = "", bool recursive = false)
		{
			if (Exists(dirPath) == false)
				throw IOException("The specified directory: '" + dirPath + "' does not exist.");

			List<string> files;

			if (recursive)
				GetFilesRecursive(dirPath, fileExt, files);
			else
			{
				List<string> localFolders;
				GetSystemItems(dirPath, fileExt, files, localFolders);
			}

			return files;
		}

		static bool Exists(const string& dirPath)
		{
			struct stat sb;
			bool exists = stat(dirPath.Ptr(), &sb) == 0 && S_ISDIR(sb.st_mode);
			return exists;
		}

		static bool Create(const string& path)
		{
		#ifdef _WIN32
			var isOK = _mkdir(path.Ptr()) != -1;
		#else
			var isOK = mkdir(path.Ptr(), S_IRWXU | S_IRWXG | S_IRWXO) != -1;
		#endif
			return isOK;
		}

	private:
		static void GetFilesRecursive(const string& dirPath, const string& fileExt,
		                              List<string>& files)
		{
			List<string> localFolders;
			GetSystemItems(dirPath, fileExt, files, localFolders);

			for (var localFolder: localFolders)
				GetFilesRecursive(localFolder, fileExt, files);
		}

		static void GetSystemItems(const string& _dirPath, const string& fileExt,
		                           List<string>& files, List<string>& folders)
		{
			string dirPath = _dirPath;
			if (!dirPath.EndsWith("/") && !dirPath.EndsWith("\\"))
				dirPath = (dirPath + "/");

			struct dirent* dp;
			DIR* dir = opendir(dirPath.Ptr());
		
			int rootPathLength = dirPath.Length();
			while ((dp = readdir(dir)) != null)
			{
				if (strcmp(dp->d_name, "..") == 0 || strcmp(dp->d_name, ".") == 0)
					continue;

				if (fileExt != "" && strstr(dp->d_name, fileExt.Ptr()) == null)
					continue;

				char fullPath[255] = { '\0' };
				strcpy(fullPath, dirPath.Ptr());
				strcpy(&fullPath[rootPathLength], dp->d_name);

				if (dp->d_type == DT_DIR)
					folders.Add(String(fullPath));
				else
					files.Add(String(fullPath));
			}

			closedir(dir);
		}
	};
}