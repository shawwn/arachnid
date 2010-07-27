//========================================================================
//	file:		gr_polygon.h
//	author:		Shawn Presser 
//	date:		7/23/10
//
// (c) 2010 Shawn Presser.  All Rights Reserved.
//========================================================================
#pragma once

//========================================================================
// Declarations
//========================================================================
class GrPolygon;
BuildVectorTypeFrom(GrPolygonVec, GrPolygon, 512, EVecObj);
//========================================================================

//========================================================================
// GrPolygon
//========================================================================
class GrPolygon
{
private:
	SMeshVertexVec		_verts;

public:
	GrPolygon(const SMeshVertex& A, const SMeshVertex& B, const SMeshVertex& C)
		: _verts(16)
	{
		_verts.reserve_exact(3);
		AddVert(A);
		AddVert(B);
		AddVert(C);
	}
	GrPolygon(const SMeshVertex* verts, uint count)
		: _verts(NextPow2(count))
	{
		_verts.assign(verts, count);
	}
	GrPolygon();
	~GrPolygon();

	uint				GetNumVerts() const					{ return (uint)_verts.size(); }
	uint				AddVert(const SMeshVertex& vert)	{ _verts.push_back(vert); return (uint)(_verts.size() - 1); }

	// provides access to the verts.
	const SMeshVertex&	GetVert(uint idx) const				{ E_ASSERT(idx < _verts.size()); return _verts[idx]; }
	SMeshVertex&		GetVert(uint idx)					{ E_ASSERT(idx < _verts.size()); return _verts[idx]; }
	SMeshVertex&		operator ()(uint idx)				{ E_ASSERT(idx < _verts.size()); return _verts[idx]; }

	// converts the polygon into a set of triangles.  Returns the number of triangles generated.
	uint				Triangulate(GrPolygonVec& tris) const;
};
//========================================================================

