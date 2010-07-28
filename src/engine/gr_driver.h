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
enum EMaterialPass;
class GrDriver;
class GrCamera;
class GrScene;
class GrMesh;
class GrTexture;
class GrLight;
class GrMaterial;
class GrSkin;

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

protected:
	bool					_isActive;

public:
	GrDriver();
	virtual ~GrDriver();

	virtual void			SetCamera(GrCamera* cam)	{ _camera = cam; }

	// returns whether the video driver has experienced a fatal error.
	virtual bool			HasFatalError() const 		{ return false; }

	// returns whether the renderer window has focus.
	virtual bool			IsWindowActive() const		{ return _isActive; }

	// scene objects.
	GrScene&				GetScene()					{ return *_scene; }
	GrCamera*				GetCamera()					{ return _camera; }

	// call at the start of each frame.
	virtual bool			BeginFrame()=0;

	// call at the end of each frame.
	virtual void			EndFrame()=0;

	// applies a camera.
	virtual void			ApplyCamera(const GrCamera& cam) { }

	// applies a light.
	virtual void			ApplyLight(const GrLight& light) { }

	// applies a material.
	virtual void			ApplyMaterial(GrMaterial* mat, EMaterialPass pass) { }

	// draws a mesh range using the current material.
	virtual void			DrawMeshRange(const MTransform& xform, GrMesh* mesh, uint triStart, uint triCount) { }

	// draws a line with the specified colors.
	virtual void			BeginLines() { }
	virtual void			DrawLines(const SColoredVertex* lines, uint count) { }

	// set the mouse position.
	virtual void			SetMousePos(int x, int y)=0;

	// mesh management.
	virtual GrMesh*			CreateMesh(const wchar_t* ctx, GrSkin* geometry)=0;

	// texture management.
	virtual GrTexture*		CreateTexture(const wchar_t* ctx, const byte* bgra, uint width, uint height)=0;
	virtual GrTexture*		CreateTexture(const wchar_t* ctx, const wstring& filePath)=0;
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