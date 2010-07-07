//========================================================================
//	file:		gr_texture.cpp
//	author:		Shawn Presser 
//	date:		7/2/10
//
// (c) 2010 Shawn Presser.  All Rights Reserved.
//========================================================================

//========================================================================
// Headers
//========================================================================
#include "e_common.h"
#include "gr_texture.h"

// graphics headers.
#include "gr_driver.h"
//========================================================================


//===================
// GrTexture::GrTexture
//===================
GrTexture::GrTexture(Internal_GrDriver* driver)
: _driver(driver)
, _userdata(NULL)
, _pixels(NULL)
, _width(NULL)
, _height(NULL)
{
	E_ASSERT(driver);
}


//===================
// GrTexture::~GrTexture
//===================
GrTexture::~GrTexture()
{
	_driver->OnDestroyTexture(*this);

	// delete texture data.
	E_DELETE_ARRAY("texture", _pixels);
}



