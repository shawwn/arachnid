//========================================================================
//	file:		e_engine.cpp
//	author:		Shawn Presser 
//	date:		6/30/10
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

// graphics headers.
#include "gr_driver.h"
//========================================================================

//========================================================================
// Definitions
//========================================================================
EEngine*	gEngine;
//========================================================================

//===================
// EEngine::EEngine
//===================
EEngine::EEngine(const wstring& rendererName)
: _renderer(NULL)
, _rendererLib(NULL)
, _rendererLibShutdownFn(NULL)
{
	if (gSystem == NULL)
		E_ERROR("engine", "ESystem not initialized.");

	if (gFileManager == NULL)
		E_ERROR("engine", "EFileManager not initialized.");

	// load the renderer.
	wstring renderer(StrLower(StrTrim(rendererName)));
	if (!renderer.empty()
		&& renderer != _T("none")
		&& renderer != _T("null"))
	{
		// build the filename of the renderer dynamic library.
		wstring rendererLibName(_T("render_"));
		rendererLibName.append(renderer);
#if E_DEBUG
		renderer.append(_T("_d"));
#endif
		// attempt to load the renderer's dynamic library from disk.
		_rendererLib = gSystem->LoadLib(rendererLibName);
		if (_rendererLib == NULL)
			E_ERROR("engine", _TS("Failed to load the renderer '") + rendererName + _TS("': The DLL '") + rendererLibName + _T("' does not exist.  Please reinstall."));

		// find the entry point and exit point.
		RendererLibStartupFunc startupFn(NULL);
		RendererLibShutdownFunc shutdownFn(NULL);

		startupFn = (RendererLibStartupFunc)gSystem->GetLibFunction(_rendererLib, GR_LIB_STARTUP_FUNC);
		shutdownFn = (RendererLibShutdownFunc)gSystem->GetLibFunction(_rendererLib, GR_LIB_SHUTDOWN_FUNC);
		if (startupFn == NULL || shutdownFn == NULL)
			E_ERROR("engine", _TS("Failed to load the renderer DLL '") + renderer + _TS("': The DLL is invalid.  Please reinstall."));

		_rendererLibShutdownFn = shutdownFn;

		// initialize the renderer.
		_renderer = (GrDriver*)(*startupFn)(GR_LIB_VERSION);
		if (_renderer == NULL)
			E_ERROR("engine", _TS("Failed to initialize the renderer.  Please reinstall.  If the problem persists, you may need to upgrade your video card."));
	}

	// unit tests.
	if (0)
	{
		// test filesystem.
		EFile* file(gFileManager->GetFile(_TS(E_PATH_ROOT) + _T("test.txt"), FILE_READ | FILE_TEXT));
		if (file != NULL)
		{
			wstrvec lines;
			while (!file->IsEOF())
				lines.push_back(file->ReadLine());
			file->Close();
			delete file;
		}

		// test matrices.
		MVec3	pt(1.0f, 0.0f, 0.0f);
		MMat33	rotX(MMat33::XRot(M_EIGTH_TURN));
		MMat33	rotY(MMat33::YRot(M_EIGTH_TURN));
		MMat33	rotZ(MMat33::ZRot(M_EIGTH_TURN));
		MMat33	rotZY(rotY * rotZ);

		rotZ.Rotate(pt);
		rotY.Rotate(pt);

		pt.Set(1.0f, 0.0f, 0.0f);
		rotZY.Rotate(pt);

		pt.Set(1.0f, 0.0f, 0.0f);
		MMat44  trans;
		trans.SetRot(rotZY);
		trans.RotateTranslate(pt);
	}

	gEngine = this;
}


//===================
// EEngine::~EEngine
//===================
EEngine::~EEngine()
{
	// shutdown the renderer.
	if (_rendererLibShutdownFn && _renderer)
		((RendererLibShutdownFunc)_rendererLibShutdownFn)(_renderer);
	_rendererLibShutdownFn = NULL;
	_renderer = NULL;

	// unload the renderer DLL.
	gSystem->UnloadLib(_rendererLib);
	_rendererLib = NULL;

	gEngine = NULL;
}

