//========================================================================
//	file:		e_system.h
//	author:		Shawn Presser 
//	date:		6/30/10
//
// (c) 2010 Shawn Presser.  All Rights Reserved.
//========================================================================
#pragma once

//========================================================================
// Declarations
//========================================================================
class EFile;
//========================================================================

//========================================================================
// ESystem
//========================================================================
class ENGINE_API ESystem
{
private:
	wstring		_commandLine;
	wstring		_rootDir;
	wstring		_userDir;
	wstring		_exePath;

public:
	ESystem();
	~ESystem();

	const wstring&		GetCommandLine() const	{ return _commandLine; }
	const wstring&		GetRootDir() const		{ return _rootDir; }
	const wstring&		GetUserDir() const		{ return _userDir; }
	const wstring&		GetExePath() const		{ return _exePath; }

	// loads a .dylib or .dll
	void*				LoadLib(const wstring& libPath);
	bool				UnloadLib(void* lib);

	// finds a function in a dynamic library
	void*				GetLibFunction(void* lib, const wstring& funcName);

	// prints a string to the debugger
	void				OutputDebug(const wstring& msg);

	// breaks execution
	void				DebugBreak();

	// immediately terminates execution
	void				Exit(int exitCode = -1);

	// immediately terminates execution with an error message
	void				CriticalError(const wstring& errMsg, int errCode = -1);

	// intentionally cause a crash
	void				Crash();

	// intentionally leak memory
	void				LeakMem(size_t numBytes);

	// pop up a message box.
	void				DisplayMessage(const wstring& caption, const wstring& msg);

	// sleep for the specified number of milliseconds
	void				Sleep(uint msec);
};
extern ENGINE_API ESystem*		gSystem;
//========================================================================