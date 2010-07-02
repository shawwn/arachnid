//========================================================================
//	file:		e_engine.h
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
class GrDriver;
//========================================================================

//========================================================================
// EEngine
//========================================================================
class ENGINE_API EEngine
{
private:
	GrDriver*			_renderer;
	void*				_rendererLib;
	void*				_rendererLibShutdownFn;
	bool				_done;
public:
	// specify renderer "none", "gl2", or "d3d9"
	EEngine(const wstring& renderer = _TS("none"));
	~EEngine();

	// call once per frame.  Returns false if the application should exit.
	bool				PerFrame();
};
extern ENGINE_API EEngine*		gEngine;
//========================================================================
