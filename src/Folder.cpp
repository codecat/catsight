#include <Folder.h>

#if defined(PLATFORM_LINUX)
#  include <sys/types.h>
#  include <sys/stat.h>
#  include <dirent.h>
#endif

FolderIndex::FolderIndex(const char* path, bool recursive)
{
	s2::string pathname = path;
	if (!pathname.endswith("/")) {
		pathname += "/";
	}
	m_path = pathname;
#if defined(PLATFORM_WINDOWS)
	ReadDir(nullptr, pathname, recursive);
#elif defined(PLATFORM_LINUX)
	ReadDir(opendir(path), pathname, recursive);
#endif
}

FolderIndex::~FolderIndex()
{
}

const char* FolderIndex::GetPath()
{
	return m_path;
}

void FolderIndex::ReadDir(void* impl, const char* dirname, bool recursive)
{
#if defined(PLATFORM_WINDOWS)
	WIN32_FIND_DATA findData;

	s2::string fnmDir = dirname;
	fnmDir += "*";

	HANDLE findHandle = FindFirstFile(fnmDir, &findData);
	if (findHandle == INVALID_HANDLE_VALUE) {
		return;
	}

	do {
		if (!strcmp(findData.cFileName, ".") || !strcmp(findData.cFileName, "..")) {
			continue;
		}

		s2::string path = dirname;
		path += findData.cFileName;

		if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			path += "/";
			m_dirs.add() = path;
			if (recursive) {
				ReadDir(nullptr, path, true);
			}
		} else {
			m_files.add() = path;
		}
	} while (FindNextFile(findHandle, &findData));

#elif defined(PLATFORM_LINUX)
	DIR* d = (DIR*)impl;
	if (!d) { return; }

	struct dirent* dir;
	while ((dir = readdir(d)) != nullptr) {
		if (!strcmp(dir->d_name, ".") || !strcmp(dir->d_name, "..")) {
			continue;
		}

		s2::string path = dirname;
		path += dir->d_name;

		struct stat fst;
		stat(path, &fst);
		if (S_ISREG(fst.st_mode)) {
			m_files.add() = path;
		} else {
			path += "/";
			m_dirs.add() = path;
			if (recursive) {
				ReadDir(opendir(path), path, true);
			}
		}
	}
#endif
}

int FolderIndex::GetFileCount()
{
	return (int)m_files.len();
}

const char* FolderIndex::GetFilePath(int i)
{
	return m_files[i];
}

int FolderIndex::GetDirCount()
{
	return (int)m_dirs.len();
}

const char* FolderIndex::GetDirPath(int i)
{
	return m_dirs[i];
}

Folder::Folder(const char* path)
{
	m_path = path;
}

Folder::~Folder()
{
}

const char* Folder::GetPath()
{
	return m_path;
}

FolderIndex Folder::GetIndex(bool recursive)
{
	return FolderIndex(m_path, recursive);
}
