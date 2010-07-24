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
class GrSkin;
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

	// skin data.
	GrSkin*				_skin;

	void*				GetUserdata() const			{ return _userdata; }

	GrMesh(Internal_GrDriver* driver);
public:
	~GrMesh();

	// provide access to vertex data.
	const SVec3*		GetPositions() const		{ return _positions; }
	const SVec2*		GetTexcoords() const		{ return _texcoords; }
	uint				GetNumVertices() const		{ return _numVertices; }

	// provide access to index data.
	const TriIdx*		GetTriIndices() const		{ return _triIndices; }
	uint				GetNumTriangles() const		{ return _numTriangles; }

	// provide access to skinning data.
	const GrSkin*		GetSkin() const				{ return _skin; }
	GrSkin*				GetSkin()					{ return _skin; }
};
//========================================================================


