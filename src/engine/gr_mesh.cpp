//========================================================================
//	file:		gr_mesh.cpp
//	author:		Shawn Presser 
//
// (c) 2010 Shawn Presser.  All Rights Reserved.
//========================================================================

//========================================================================
// Headers
//========================================================================
#include "e_common.h"
#include "gr_mesh.h"

// graphics headers.
#include "gr_driver.h"
#include "gr_skin.h"
//========================================================================


//===================
// GrMesh::GrMesh
//===================
GrMesh::GrMesh(Internal_GrDriver* driver)
: _driver(driver)
, _userdata(NULL)
, _positions(NULL)
, _texcoords(NULL)
, _numVertices(0)
, _triIndices(NULL)
, _numTriangles(0)
, _skin(NULL)
{
	E_ASSERT(driver);
}


//===================
// GrMesh::~GrMesh
//===================
GrMesh::~GrMesh()
{
	_driver->OnDestroyMesh(*this);

	// delete mesh data.
	E_DELETE_ARRAY("mesh", _positions);
	E_DELETE_ARRAY("mesh", _texcoords);
	E_DELETE_ARRAY("mesh", _triIndices);

	// delete skin data.
	E_DELETE("mesh", _skin);
}


