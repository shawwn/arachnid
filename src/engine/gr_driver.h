//========================================================================
//	file:		gr_driver.h
//	author:		Shawn Presser 
//	date:		7/1/10
//
// (c) 2010 Shawn Presser.  All Rights Reserved.
//========================================================================
#pragma once

//========================================================================
// Declarations
//========================================================================
class GrDriver;

#define GR_LIB_VERSION				1

#define GR_LIB_STARTUP_FUNC			_T("RendererStartup")
typedef void*						(*RendererLibStartupFunc)(int version,
															  int windowWidth, int windowHeight,
															  const wstring& windowTitle);

#define GR_LIB_SHUTDOWN_FUNC		_T("RendererShutdown")
typedef bool						(*RendererLibShutdownFunc)(void* driver);
//========================================================================

//========================================================================
// GrDriver
//		each graphics driver function is defined for D3D9, D3D10,
//		GL2, GL3, etc, allowing the engine to be cross-platform.
//========================================================================
class ENGINE_API GrDriver
{
public:
	virtual ~GrDriver() { }

	// returns whether the video driver has experienced a fatal error.
	virtual bool			HasFatalError()		{ return false; }

	// call at the start of each frame.
	virtual bool			BeginFrame()=0;

	// render the scene from the camera's point of view.
	virtual void			Render()=0;

	// call at the end of each frame.
	virtual void			EndFrame()=0;
};
//========================================================================
