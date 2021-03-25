#pragma once

#include <Common.h>

class Folder;

class FolderIndex
{
	friend class Folder;

public:
	FolderIndex(const char* path, bool recursive);
	~FolderIndex();

	const char* GetPath();

	int GetFileCount();
	const char* GetFilePath(int i);

	int GetDirCount();
	const char* GetDirPath(int i);

private:
	void ReadDir(void* impl, const char* dirname, bool recursive);

	s2::string m_path;
	s2::list<s2::string> m_files;
	s2::list<s2::string> m_dirs;
};

class Folder
{
public:
	Folder(const char* path);
	~Folder();

	const char* GetPath();

	FolderIndex GetIndex(bool recursive = false);

private:
	s2::string m_path;
};
