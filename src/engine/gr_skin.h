//========================================================================
//	file:		gr_skin.h
//	author:		Shawn Presser 
//	date:		7/12/10
//
// (c) 2010 Shawn Presser.  All Rights Reserved.
//========================================================================
#pragma once

//========================================================================
// Declarations
//========================================================================
struct GrSkin_impl; 
class MTransform;
//========================================================================

//========================================================================
// GrSkin
//========================================================================
class ENGINE_API GrSkin
{
private:
	GrSkin_impl*	_impl;
	float			_curTotalWeight;
	uint			_channels;
	bool			_dirty;

	void			RecomputeTangentsBinormals();
	void			NormalizeTBN();

public:
	GrSkin(uint reserveVerts = 0, uint reserveTris = 0);
	~GrSkin();

	uint			GetNumVerts() const;
	uint			GetNumTris() const;
	uint			GetMeshChannels() const	{ return _channels; }
	bool			GetDeformed() const		{ return _dirty; }
	void			ClearDeformed()			{ _dirty = false; }

	TriIdx*			GetIndices() const;

	SVec3*			GetTangents() const;
	SVec3*			GetBinormals() const;
	SVec3*			GetNormals() const;

	SVec3*			GetPositions() const;

	SVec2*			GetTexcoords() const;

	SVec3*			DeformVerts(const MTransform** bones);

	void			Reset();

	void			StartVert(const SVec3& pos, const SVec2& uv);
	void			AddNormals(const SVec3& normal);
	void			AddTangents(const SVec3& uBasis, const SVec3& vBasis);
	void			AddBoneWeight(byte boneIndex, float weight);

	void			AddTriangle(TriIdx idxA, TriIdx idxB, TriIdx idxC);
};
//========================================================================

