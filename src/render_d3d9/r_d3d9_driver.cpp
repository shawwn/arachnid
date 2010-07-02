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
#include "../engine/gr_driver.h"
//========================================================================

//========================================================================
// D3D9Driver
//========================================================================
class D3D9Driver : public GrDriver
{
public:
	//===================
	// D3D9::BeginFrame
	//===================
	bool		BeginFrame()
	{
		return true;
	}


	//===================
	// D3D9::Render
	//===================
	void		Render()
	{
	}

	
	//===================
	// D3D9::EndFrame
	//===================
	void		EndFrame()
	{
	}
};

//===================
// RendererStartup
//===================
RENDER_D3D9_EXPORT void*		RendererStartup(int version,
												int windowWidth, int windowHeight,
												const wstring& windowTitle)
{
	E_VERIFY(version == GR_LIB_VERSION, return NULL);

	// return an interface to the driver.
	GrDriver* driver(E_NEW("d3d9", D3D9Driver));
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

