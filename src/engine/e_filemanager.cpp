//========================================================================
//	file:		e_filemanager.cpp
//	author:		Shawn Presser 
//	date:		6/30/10
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
// EFileManager::Exists
//===================
bool		EFileManager::Exists(const wstring& filePath)
{
	wstring rootFilePath(GetAbsolutePath(filePath));
	if (_waccess(rootFilePath.c_str(), 0) == -1)
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
	case E_PATH_ROOT:
		path = path.substr(1);
		if (!StrBeginsWith(path, gSystem->GetRootDir()))
			path = gSystem->GetRootDir() + path;
		break;

		// use user dir (e.g. "c:/users/shawn/")
	case E_PATH_USER:
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

	if (!Exists(absolutePath))
		return NULL;

	EFile* file(new EFileDisk());
	if (!file->Open(absolutePath, mode))
	{
		delete file;
		return NULL;
	}

	return file;
}

