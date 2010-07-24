//========================================================================
//	file:		e_engine.cpp
//	author:		Shawn Presser 
//
// (c) 2010 Shawn Presser.  All Rights Reserved.
//========================================================================

//========================================================================
// Headers
//========================================================================
#include "e_common.h"
#include "e_engine.h"

// engine headers.
#include "e_system.h"
#include "e_filemanager.h"

// math headers.
#include "m_vec3.h"
#include "m_mat33.h"
#include "m_mat44.h"
#include "m_transform.h"

// graphics headers.
#include "gr_driver.h"
#include "gr_camera.h"
#include "gr_scene.h"
#include "gr_model.h"
//========================================================================

//========================================================================
// Definitions
//========================================================================
EEngine*	gEngine;
//========================================================================

//===================
// EEngine::CenterMouse
//===================
void				EEngine::CenterMouse()
{
	_renderer->SetMousePos(_windowWidth/2, _windowHeight/2);
	_mouseX = _windowWidth/2;
	_mouseY = _windowHeight/2;
}


//===================
// EEngine::EEngine
//===================
EEngine::EEngine()
	: _renderer(NULL)
	, _rendererLib(NULL)
	, _rendererLibShutdownFn(NULL)
	, _cam(NULL)
	, _windowWidth(E_DEFAULT_WINDOW_WIDTH)
	, _windowHeight(E_DEFAULT_WINDOW_HEIGHT)
	, _mouseX(_windowWidth/2)
	, _mouseY(_windowHeight/2)
	, _done(false)
	, _wasActive(false)
{
}


//===================
// EEngine::Create
//===================
EEngine*			EEngine::Create(const wstring& ctx, const SEngineConfig& config)
{
	E_VALIDATE(gSystem != NULL,
		"engine", "system not initialized.",
		return NULL);

	E_VALIDATE(gFileManager != NULL,
		"engine", "filesystem not initialized.",
		return NULL);

	E_VALIDATE(gEngine == NULL,
		"engine", "engine already initialized.",
		return NULL);

	EEngine* result(E_NEW(ctx, EEngine));
	gEngine = result;

	// create the camera.
	result->_cam = E_NEW("engine", GrFirstPersonCamera)(config.side, config.up, config.forward);

	// load the renderer.
	wstring renderer(StrLower(StrTrim(config.renderer)));
	if (!renderer.empty()
		&& renderer != _T("none")
		&& renderer != _T("null"))
	{
		// build the filename of the renderer dynamic library.
		wstring rendererLibName(_T("render_"));
		rendererLibName.append(renderer);
#if E_DEBUG
		rendererLibName.append(_T("_d"));
#endif
		// attempt to load the renderer's dynamic library from disk.
		result->_rendererLib = gSystem->LoadLib(rendererLibName);
		E_VALIDATE(result->_rendererLib != NULL,
			"engine", _TS("Failed to load the renderer '") + config.renderer + _TS("': The DLL '") + rendererLibName + _T("' does not exist.  Please reinstall."),
		{
			gEngine = NULL;
			E_DELETE("engine", result);
			return NULL;
		});

		// find the entry point and exit point.
		RendererLibStartupFunc startupFn(NULL);
		RendererLibShutdownFunc shutdownFn(NULL);

		startupFn = (RendererLibStartupFunc)gSystem->GetLibFunction(result->_rendererLib, GR_LIB_STARTUP_FUNC);
		shutdownFn = (RendererLibShutdownFunc)gSystem->GetLibFunction(result->_rendererLib, GR_LIB_SHUTDOWN_FUNC);
		E_VALIDATE(startupFn != NULL && shutdownFn != NULL,
			"engine", _TS("Failed to load the renderer DLL '") + renderer + _TS("': The DLL is invalid.  Please reinstall."),
		{
			gEngine = NULL;
			E_DELETE("engine", result);
			return NULL;
		});

		result->_rendererLibShutdownFn = shutdownFn;

		// initialize the renderer.
		result->_renderer = (GrDriver*)(*startupFn)(GR_LIB_VERSION,
			E_DEFAULT_WINDOW_WIDTH, E_DEFAULT_WINDOW_HEIGHT,
			E_DEFAULT_WINDOW_TITLE);
		E_VALIDATE(result->_renderer != NULL,
			"engine", _TS("Failed to initialize the renderer.  Please reinstall.  If the problem persists, you may need to upgrade your video card."),
		{
			gEngine = NULL;
			E_DELETE("engine", result);
			return NULL;
		});

		// set the camera.
		result->_renderer->SetCamera(result->_cam);
	}

	// return the engine.
	return result;
}


//===================
// EEngine::~EEngine
//===================
EEngine::~EEngine()
{
	// shutdown the renderer.
	if (_rendererLibShutdownFn && _renderer)
		((RendererLibShutdownFunc)_rendererLibShutdownFn)(_renderer);

	// unload the renderer DLL.
	gSystem->UnloadLib(_rendererLib);

	// delete the camera.
	E_DELETE("engine", _cam);

	if (gEngine == this)
		gEngine = NULL;
}


//===================
// EEngine::PerFrame
//===================
bool				EEngine::PerFrame(uint dt)
{
	if (_done)
		return true;

	if (_renderer != NULL)
	{
		_renderer->GetScene().GetModel().Update(dt);
		
		GrFirstPersonCamera* cam((GrFirstPersonCamera*)_cam);

		// update the renderer.
		if (!_renderer->BeginFrame())
		{
			_done = true;
			return false;
		}
		_renderer->EndFrame();

		bool isActive = _renderer->IsWindowActive();
		if (isActive && !_wasActive)
			CenterMouse();
		_wasActive = isActive;

		if (isActive)
		{
			// rotate the camera.
			if (_mouseX != _windowWidth/2 || _mouseY != _windowHeight/2)
			{
				float dX = ((int)_windowWidth/2 - _mouseX) / (float)_windowWidth;
				float dY = ((int)_windowHeight/2 - _mouseY) / (float)_windowHeight;

				cam->YawTilt(TURNS_TO_ANGLE(dX), TURNS_TO_ANGLE(dY));
				CenterMouse();
			}

			// move the camera.
			const float speed = 100.0f;
			float t = speed * (dt / 1000.0f);

			MVec3 impulse(0.0f, 0.0f, 0.0f);

			if (gSystem->IsKeyPressed('w'))
				impulse.Z() += t;
			if (gSystem->IsKeyPressed('s'))
				impulse.Z() -= t;

			if (gSystem->IsKeyPressed('d'))
				impulse.X() += t;
			if (gSystem->IsKeyPressed('a'))
				impulse.X() -= t;

			if (gSystem->IsKeyPressed(' '))
				impulse.Y() += t;
			if (gSystem->IsKeyPressed('c'))
				impulse.Y() -= t;
			cam->Impulse(impulse);

			if (gSystem->IsKeyPressed(0x1B)) // Escape
				return false;
		}
	}

	return true;
}


//===================
// EEngine::OnResize
//===================
void				EEngine::OnResize(uint windowWidth, uint windowHeight)
{
	_windowWidth = windowWidth;
	_windowHeight = windowHeight;
}


//===================
// EEngine::OnMousePos
//===================
void				EEngine::OnMousePos(int x, int y)
{
	_mouseX = x;
	_mouseY = y;
}

