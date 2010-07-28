//========================================================================
//	file:		e_engine.h
//	author:		Shawn Presser 
//	date:		6/30/10
//
// (c) 2010 Shawn Presser.  All Rights Reserved.
//========================================================================
#pragma once

//========================================================================
// Headers
//========================================================================
#include "gr_driver.h"

// math headers.
#include "m_vec3.h"
//========================================================================

//========================================================================
// Declarations
//========================================================================
class EFile;
class GrScene;
class GrCamera;
class GrRenderAmbient;
class GrRenderLight;
//========================================================================

//========================================================================
// SEngineConfig
//========================================================================
struct SEngineConfig
{
	MVec3			side;
	MVec3			up;
	MVec3			forward;

	wstring			renderer;

	SEngineConfig()
		: side(1.0f, 0.0f, 0.0f)
		, up(0.0f, 1.0f, 0.0f)
		, forward(0.0f, 0.0f, -1.0f)
		, renderer(_TS("none"))
	{
	}
};
//========================================================================

//========================================================================
// EEngine
//========================================================================
class ENGINE_API EEngine
{
private:
	GrDriver*				_renderer;
	void*					_rendererLib;
	void*					_rendererLibShutdownFn;

	GrCamera*				_cam;

	GrRenderAmbient*		_renderAmbient;
	GrRenderLight*			_renderLight;

	uint					_windowWidth;
	uint					_windowHeight;

	uint					_time;

	int						_mouseX;
	int						_mouseY;

	bool					_done;
	bool					_wasActive;

	void					CenterMouse();

	EEngine();
public:
	// specify renderer "none", "gl2", or "d3d9"
	static EEngine*			Create(const wstring& ctx, const SEngineConfig& config = SEngineConfig());
	~EEngine();

	// provides access to the scene objects;
	GrDriver&				GetRenderer()					{ return *_renderer; }
	GrScene&				GetScene()						{ return _renderer->GetScene(); }
	GrCamera&				GetCamera()						{ return *_cam; }

	// call once per frame, passing in the number of milliseconds since the last frame.
	// Returns false if the application should exit.
	bool					PerFrame(uint dt);

	// internal use only.
	void					OnRender();
	void					OnResize(uint windowWidth, uint windowHeight);
	void					OnMousePos(int x, int y);
};
extern ENGINE_API EEngine*		gEngine;
//========================================================================
