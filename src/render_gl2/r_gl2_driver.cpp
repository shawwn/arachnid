//========================================================================
//	file:		r_gl2_driver.cpp
//	author:		Shawn Presser 
//	date:		7/1/10
//
// (c) 2010 Shawn Presser.  All Rights Reserved.
//========================================================================

//========================================================================
// Headers
//========================================================================
#include "r_gl2_common.h"
#include "../engine/gr_driver.h"
//========================================================================

//========================================================================
// GL2Driver
//========================================================================
class GL2Driver : public GrDriver
{
public:
	void		Render()
	{
	}
};

//===================
// RendererStartup
//===================
RENDER_GL2_EXPORT void*			RendererStartup(int version)
{
	E_VERIFY(version == GR_LIB_VERSION, return NULL);

	GrDriver* driver(E_NEW("gl2", GL2Driver));
	return driver;
}

//===================
// RendererShutdown
//===================
RENDER_GL2_EXPORT bool			RendererShutdown(void* driverPtr)
{
	GrDriver* driver((GrDriver*)driverPtr);
	E_DELETE("gl2", driver);
	return true;
}

