//========================================================================
//	file:		gr_polygon.cpp
//	author:		Shawn Presser 
//
// (c) 2010 Shawn Presser.  All Rights Reserved.
//========================================================================

//========================================================================
// Headers
//========================================================================
#include "e_common.h"
#include "gr_polygon.h"
//========================================================================

//========================================================================
// GrPolygon
//========================================================================

//===================
// GrPolygon::GrPolygon
//===================
GrPolygon::GrPolygon()
: _verts(16)
{
}


//===================
// GrPolygon::~GrPolygon
//===================
GrPolygon::~GrPolygon()
{
}


//===================
// GrPolygon::~GrPolygon
//===================
uint				GrPolygon::Triangulate(GrPolygonVec& tris) const
{
	uint numVerts(_verts.size());
	E_VERIFY(numVerts >= 3, return 0);

	const SMeshVertex& A(_verts[0]);
	for (uint i = 1; i < numVerts - 2; ++i)
	{
		const SMeshVertex& B(_verts[i]);
		const SMeshVertex& C(_verts[i + 1]);
		tris.push_back(GrPolygon(A, B, C));
	}

	return numVerts - 2;
}
//========================================================================
