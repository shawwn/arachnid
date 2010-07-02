//========================================================================
//	file:		e_filemanager.h
//	author:		Shawn Presser 
//	date:		6/30/10
//
// (c) 2010 Shawn Presser.  All Rights Reserved.
//========================================================================
#pragma once

//========================================================================
// Headers
//========================================================================
#include "e_file.h"
//========================================================================

//========================================================================
// Constants
//========================================================================
#define E_PATH_ROOT		_T('#')
#define E_PATH_USER		_T('~')
//========================================================================

//========================================================================
// EFileManager
//========================================================================
class ENGINE_API EFileManager
{
public:
	EFileManager();
	~EFileManager();

	static wstring		SanitizePath(const wstring& path);

	bool				Exists(const wstring& filePath);

	wstring				GetAbsolutePath(const wstring& path);

	EFile*				GetFile(const wstring& path, uint mode);
};
extern ENGINE_API EFileManager*		gFileManager;
//========================================================================