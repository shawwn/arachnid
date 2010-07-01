//========================================================================
//	file:		e_system.cpp
//	author:		Shawn Presser 
//	date:		6/30/10
//
// (c) 2010 Shawn Presser.  All Rights Reserved.
//========================================================================

//========================================================================
// Headers
//========================================================================
#include "e_common.h"
#include "e_system.h"
#include "e_filemanager.h"
#include <windows.h>
#undef _T
#include <tchar.h>
#include <shlobj.h>
//========================================================================

//========================================================================
// Definitions
//========================================================================
ESystem*	gSystem;
//========================================================================


//===================
// ESystem::ESystem
//===================
ESystem::ESystem()
{
	// store the command line.
	_commandLine = GetCommandLine();

	// store the root path
	wchar_t dir[MAX_PATH + 1];
	GetModuleFileName(NULL, dir, MAX_PATH); 
	_rootDir = EFileManager::SanitizePath(dir);

	// strip off the executable name.
	size_t pos(_rootDir.find_last_of(_T('/')));
	if (pos != wstring::npos)
		_rootDir = _rootDir.substr(0, pos+1);

	// store the user path.
	SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, dir);
	_userDir = EFileManager::SanitizePath(dir);
	_userDir += _T("/");

	gSystem = this;
}


//===================
// ESystem::~ESystem
//===================
ESystem::~ESystem()
{
	gSystem = NULL;
}

