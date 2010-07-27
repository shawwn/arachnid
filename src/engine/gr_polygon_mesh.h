//========================================================================
//	file:		gr_polygon_mesh.h
//	author:		Shawn Presser 
//	date:		7/23/10
//
// (c) 2010 Shawn Presser.  All Rights Reserved.
//========================================================================
#pragma once

//========================================================================
// Headers
//========================================================================
// graphics headers.
#include "gr_polygon.h"
//========================================================================

//========================================================================
// Declarations
//========================================================================
class GrSkin;
//========================================================================

//========================================================================
// GrPolygonMesh
//========================================================================
class ENGINE_API GrPolygonMesh
{
private:
	GrPolygonVec			_polys;
	GrPolygonVec			_triangles;

	uint					_channels;

	void					Triangulate();

public:
	GrPolygonMesh();
	~GrPolygonMesh();

	void					AddPoly(const GrPolygon& poly)		{ _polys.push_back(poly); }

	void					SetChannels(uint channels)			{ _channels |= channels; }
	void					ClearChannels(uint channels)		{ _channels &= ~channels; }
	bool					HasChannels(uint channels) const	{ return HAS_FLAGS(_channels, channels); }

	GrSkin*					BuildIndexedGeometry();
};
//========================================================================



