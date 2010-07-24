//========================================================================
//	file:		gr_driver.cpp
//	author:		Shawn Presser 
//
// (c) 2010 Shawn Presser.  All Rights Reserved.
//========================================================================

//========================================================================
// Headers
//========================================================================
#include "e_common.h"
#include "gr_driver.h"

// graphics headers.
#include "gr_scene.h"
#include "gr_camera.h"
//========================================================================


//===================
// GrDriver::GrDriver
//===================
GrDriver::GrDriver()
: _scene(E_NEW("renderer", GrScene)(_T("global")))
, _camera(NULL)
, _isActive(false)
{
}


//===================
// GrDriver::~GrDriver
//===================
GrDriver::~GrDriver()
{
	E_DELETE("renderer", _scene);
}