//========================================================================
//	file:		r_d3d9_driver.cpp
//	author:		Shawn Presser 
//	date:		7/1/10
//
// (c) 2010 Shawn Presser.  All Rights Reserved.
//========================================================================

//========================================================================
// Headers
//========================================================================
#include "r_d3d9_common.h"
#include "r_d3d9_driver.h"
#include "../engine/gr_driver.h"
#include "../engine/gr_scene.h"
#include "../engine/gr_camera.h"
//========================================================================


//===================
// D3D9Driver::D3D9Driver
//===================
D3D9Driver::D3D9Driver(int windowWidth, int windowHeight, const wstring& windowTitle)
: _fatalError(false)
, _winIdx(0)
{
	// validate parameters.
	windowWidth = E_MAX(640, windowWidth);
	windowHeight = E_MAX(480, windowWidth);
}


//===================
// D3D9Driver::~D3D9Driver
//===================
D3D9Driver::~D3D9Driver()
{
}


//===================
// D3D9Driver::BeginFrame
//===================
bool		D3D9Driver::BeginFrame()
{
	return true;
}


//===================
// D3D9Driver::Render
//===================
void		D3D9Driver::Render(const GrScene& scene, const GrCamera& camera)
{
}


//===================
// D3D9Driver::EndFrame
//===================
void		D3D9Driver::EndFrame()
{
}


//===================
// RendererStartup
//===================
RENDER_D3D9_EXPORT void*		RendererStartup(int version,
												int windowWidth, int windowHeight,
												const wstring& windowTitle)
{
	E_VERIFY(version == GR_LIB_VERSION, return NULL);

	// return an interface to the driver.
	GrDriver* driver(E_NEW("d3d9", D3D9Driver)(windowWidth, windowHeight, windowTitle));
	return driver;
}


//===================
// RendererShutdown
//===================
RENDER_D3D9_EXPORT bool			RendererShutdown(void* driverPtr)
{
	GrDriver* driver((GrDriver*)driverPtr);
	E_DELETE("d3d9", driver);
	return true;
}

