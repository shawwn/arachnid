//========================================================================
//	file:		gr_mesh.h
//	author:		Shawn Presser 
//	date:		7/2/10
//
// (c) 2010 Shawn Presser.  All Rights Reserved.
//========================================================================
#pragma once

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

	// vertex data.
	SVec3*				_positions;
	SVec2*				_texcoords;
	uint				_numVertices;

	// index data.
	TriIdx*				_triIndices;
	uint				_numTriangles;

	void*				GetUserdata() const			{ return _userdata; }

	GrMesh(Internal_GrDriver* driver);
public:
	~GrMesh();

	// provide access to vertex data.
	SVec3*				GetPositions() const		{ return _positions; }
	SVec2*				GetTexcoords() const		{ return _texcoords; }
	uint				GetNumVertices() const		{ return _numVertices; }

	// provide access to index data.
	TriIdx*				GetTriIndices() const		{ return _triIndices; }
	uint				GetNumTriangles() const		{ return _numTriangles; }
};
//========================================================================


