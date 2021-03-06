//========================================================================
//	file:		gr_mesh.h
//	author:		Shawn Presser 
//	date:		7/2/10
//
// (c) 2010 Shawn Presser.  All Rights Reserved.
//========================================================================
#pragma once

//========================================================================
// Headers
//========================================================================
#include "gr_skin.h"
//========================================================================

//========================================================================
// Declarations
//========================================================================
class GL2Driver;
class D3D9Driver;
class Internal_GrDriver;
//========================================================================

//========================================================================
// GrMesh
//========================================================================
class ENGINE_API GrMesh
{
	friend class GL2Driver;
	friend class D3D9Driver;
private:
	Internal_GrDriver*	_driver;
	void*				_userdata;

	// geometry data.
	GrSkin*				_skin;

	void*				GetUserdata() const			{ return _userdata; }

	GrMesh(Internal_GrDriver* driver);
public:
	~GrMesh();

	// provide access to the geometry.
	const GrSkin*		GetSkin() const				{ return _skin; }
	GrSkin*				GetSkin()					{ return _skin; }
};
//========================================================================


