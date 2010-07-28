//========================================================================
//	file:		e_filemanager.cpp
//	author:		Shawn Presser 
//
// (c) 2010 Shawn Presser.  All Rights Reserved.
//========================================================================

//========================================================================
// Headers
//========================================================================
#include "e_common.h"
#include "e_filemanager.h"
#include "e_system.h"
#include "e_filedisk.h"
#include "e_filemem.h"
//========================================================================

//========================================================================
// Definitions
//========================================================================
EFileManager*	gFileManager;
//========================================================================

//===================
// EFileManager::EFileManager
//===================
EFileManager::EFileManager()
{
	gFileManager = this;
}


//===================
// EFileManager::~EFileManager
//===================
EFileManager::~EFileManager()
{
	gFileManager = NULL;
}


//===================
// EFileDisk::SanitizePath
//===================
wstring		EFileManager::SanitizePath(const wstring& path)
{
	wstring sUsePath(StrLower(StrTrim(path)));

	// replace back slashes with forward slashes.
	sUsePath = StrReplace(sUsePath, _T("\\"), _T("/"));

	// replace a sequence of forward slashes with a single forward slash.
	sUsePath = StrCompact(sUsePath, _T("/"), _T("/"));

	// replace a sequence of "....." with ".."
	sUsePath = StrCompact(sUsePath, _T("."), _T(".."), 2);

	// a path may not contain quotes.
	sUsePath = StrReplace(sUsePath, _T("\""), _T(""));

	return sUsePath;
}


//===================
// EFileManager::ParsePath
//===================
void		EFileManager::ParsePath(wstring& inOutPath, wstring& outName, wstring& outExtension)
{
	inOutPath = GetAbsolutePath(inOutPath);
	outName = StrAfterLast(inOutPath, _T("/"));
	outName = StrBeforeLast(outName, _T("."), &outExtension);
}


//===================
// EFileManager::Exists
//===================
bool		EFileManager::Exists(const wstring& filePath)
{
	wstring rootFilePath(GetAbsolutePath(filePath));
	int access(_waccess(rootFilePath.c_str(), 0));
	if (access == -1)
		return false;

	return true;
}


//===================
// EFileManager::GetSystemPath
//===================
wstring		EFileManager::GetAbsolutePath(const wstring& dirtyPath)
{
	wstring path(dirtyPath);
	if (path.empty())
		return WSNULL;

	switch (path[0])
	{
		// use root dir (e.g. "c:/arachnid/")
	case FILE_PATH_ROOT_DIR:
		path = path.substr(1);
		if (!StrBeginsWith(path, gSystem->GetRootDir()))
			path = gSystem->GetRootDir() + path;
		break;

		// use user dir (e.g. "c:/users/shawn/")
	case FILE_PATH_USER_DIR:
		path = path.substr(1);
		if (!StrBeginsWith(path, gSystem->GetUserDir()))
			path = gSystem->GetUserDir() + path;
		break;
	}

	path = SanitizePath(path);
	return path;
}


//===================
// EFileManager::GetFile
//===================
EFile*		EFileManager::GetFile(const wstring& path, uint mode)
{
	wstring absolutePath(GetAbsolutePath(path));

	if ((mode & FILE_READ) != 0)
		if (!Exists(absolutePath))
			return NULL;

	EFile* file(NULL);
	
	if ((mode & FILE_MEMORY) != 0)
		file = E_NEW("fileManager",EFileMem)();
	else
		file = E_NEW("fileManager",EFileDisk)();

	if (!file->Open(absolutePath, mode))
	{
		E_DELETE("fileManager", file);
		return NULL;
	}

	return file;
}

