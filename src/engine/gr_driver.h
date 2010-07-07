//========================================================================
//	file:		gr_driver.h
//	author:		Shawn Presser 
//	date:		7/1/10
//
// (c) 2010 Shawn Presser.  All Rights Reserved.
//========================================================================
#pragma once

//========================================================================
// Headers
//========================================================================
#include "gr_types.h"
//========================================================================

//========================================================================
// Declarations
//========================================================================
class GrDriver;
class GrCamera;
class GrScene;
class GrMesh;
class GrTexture;

// renderer DLL functions.
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
private:
	GrScene*				_scene;
	GrCamera*				_camera;

public:
	GrDriver();
	virtual ~GrDriver();

	// returns whether the video driver has experienced a fatal error.
	virtual bool			HasFatalError() const 		{ return false; }

	// scene objects.
	GrScene&				GetScene()					{ return *_scene; }
	GrCamera&				GetCamera()					{ return *_camera; }

	// call at the start of each frame.
	virtual bool			BeginFrame()=0;

	// call at the end of each frame.
	virtual void			EndFrame()=0;

	// set the mouse position.
	virtual void			SetMousePos(int x, int y)=0;

	// mesh management.
	virtual GrMesh*			CreateMesh(const wchar_t* ctx,
		const SVec3* positions, const SVec2* texcoords, uint numVerts,
		const TriIdx* triangles, uint numTris)=0;

	// texture management.
	virtual GrTexture*		CreateTexture(const wchar_t* ctx, const byte* bgra, uint width, uint height)=0;
};
//========================================================================

//========================================================================
// GrDriverInternal
//		The internal GrDriver API.
//========================================================================
class ENGINE_API Internal_GrDriver : public GrDriver
{
public:
	virtual void			OnDestroyMesh(GrMesh& mesh)=0;
	virtual void			OnDestroyTexture(GrTexture& texture)=0;
};
//========================================================================