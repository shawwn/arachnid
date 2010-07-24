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
#include "e_lean_windows.h"
#include <shlobj.h> // SHGetFolderPath
#include <mmsystem.h> // OutputDebugString
//========================================================================

//========================================================================
// Libraries
//========================================================================
#pragma comment(lib, "winmm.lib")
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
: _timeStart(uint(-1))
{
	// store the command line.
	_commandLine = GetCommandLine();

	// store the exe path
	wchar_t dir[MAX_PATH + 1];
	GetModuleFileName(NULL, dir, MAX_PATH); 
	_exePath = EFileManager::SanitizePath(dir);

	// strip off the executable name to get the root path
	_rootDir = _exePath;
	size_t pos(_rootDir.find_last_of(_T('/')));
	if (pos != wstring::npos)
		_rootDir = _rootDir.substr(0, pos+1);

	// store the user path.
	SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, dir);
	_userDir = EFileManager::SanitizePath(dir);
	_userDir += _T("/");

	// make TimeElapsed be relative to now.
	_timeStart = timeGetTime();

	gSystem = this;
}


//===================
// ESystem::~ESystem
//===================
ESystem::~ESystem()
{
	gSystem = NULL;
}


//===================
// ESystem::OutputDebug
//===================
void				ESystem::OutputDebug(const wstring& msg)
{
	OutputDebugStr(msg.c_str());
}


//===================
// ESystem::DebugBreak
//===================
void				ESystem::DebugBreak()
{
	__asm
	{
		int 3
	}
}


//===================
// ESystem::LoadLib
//===================
void*				ESystem::LoadLib(const wstring& libPath)
{
	// attempt to load the library.
	HMODULE module(LoadLibrary((libPath + _T(".dll")).c_str()));

	// return a handle to the module.
	return module;
}


//===================
// ESystem::UnloadLib
//===================
bool				ESystem::UnloadLib(void* lib)
{
	HMODULE module((HMODULE)lib);
	if (module == NULL)
		return false;

	if (FreeLibrary(module) == 0)
		return false;

	return true;
}


//===================
// ESystem::GetLibFunction
//===================
void*				ESystem::GetLibFunction(void* libHandle, const wstring& libFuncName)
{
	HMODULE module((HMODULE)libHandle);
	E_VERIFY(module != NULL, return NULL);

	string funcName(WStringToString(StrTrim(libFuncName)));
	E_VERIFY(!funcName.empty(), return NULL);

	return GetProcAddress(module, funcName.c_str());
}


//===================
// ESystem::Exit
//===================
void				ESystem::Exit(int exitCode)
{
	exit(exitCode);
	E_WARN("system", "should not get here");
	Crash();
}


//===================
// ESystem::CriticalError
//===================
void				ESystem::CriticalError(const wstring& errMsg, int errCode)
{
	OutputDebug(_T("****CRITICAL ERROR**** "));
	OutputDebug(errMsg);
	exit(errCode);
}


//===================
// ESystem::Crash
//===================
void				ESystem::Crash()
{
	// crash safely.
	int* badPtr(NULL);
	for (int i = 0; i < 1000; ++i)
		badPtr[i] = badPtr[i];
}


//===================
// ESystem::LeakMem
//===================
void				ESystem::LeakMem(size_t numBytes)
{
	malloc(numBytes);
}


//===================
// ESystem::DisplayMessage
//===================
void				ESystem::DisplayMessage(const wstring& caption, const wstring& msg)
{
	::MessageBox(NULL, msg.c_str(), caption.c_str(), MB_OK);
}


//===================
// ESystem::Sleep
//===================
void				ESystem::Sleep(uint msec)
{
	::Sleep(msec);
}


//===================
// ESystem::Warn
//===================
void				ESystem::Warn(const wstring& context, const wstring& msg)
{
	DisplayMessage(context, msg);
}


//===================
// ESystem::ElapsedTime
//===================
uint				ESystem::TimeElapsed()
{
	E_VERIFY(_timeStart != uint(-1), return 0);
	return timeGetTime() - _timeStart;
}


//===================
// ESystem::TimeStart
//===================
void				ESystem::TimeStart()
{
	_timeStart = timeGetTime();
}


//===================
// ESystem::IsKeyPressed
//===================
bool				ESystem::IsKeyPressed(char c)
{
	c = toupper(c);
	SHORT s(GetAsyncKeyState((int)c));

	// "if the most significant bit is set, the key is down"
	if (s & (1 << (8*sizeof(SHORT) - 1)))
		return true;

	return false;
}

