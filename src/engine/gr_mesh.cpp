//========================================================================
//	file:		gr_mesh.cpp
//	author:		Shawn Presser 
//	date:		7/2/10
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
}


