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

public:
	ESystem();
	~ESystem();

	const wstring&		GetCommandLine() const	{ return _commandLine; }
	const wstring&		GetRootDir() const		{ return _rootDir; }
	const wstring&		GetUserDir() const		{ return _userDir; }
};
extern ENGINE_API ESystem*		gSystem;
//========================================================================