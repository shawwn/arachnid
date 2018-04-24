//========================================================================
//	file:		gr_skin.cpp
//	author:		Shawn Presser 
//
// (c) 2010 Shawn Presser.  All Rights Reserved.
//========================================================================

//========================================================================
// Headers
//========================================================================
#include "e_common.h"
#include "gr_skin.h"

// math headers.
#include "m_transform.h"
//========================================================================

//========================================================================
// struct SBoneVert
//========================================================================
struct SBoneVert
{
	float		weight;
	SVec3		xyz;

	SVec3		tangent;
	SVec3		binormal;
	SVec3		normal;

	SBoneVert()
		: weight(0.0f)
		, xyz(SVec3::Zero)
		, tangent(SVec3::Zero)
		, binormal(SVec3::Zero)
		, normal(SVec3::Zero)
	{
	}

	SBoneVert(float weight, const SVec3& xyz, const SVec3& normal)
		: weight(weight)
		, xyz(xyz)
		, tangent(SVec3::Zero)
		, binormal(SVec3::Zero)
		, normal(normal)
	{
	}

	SBoneVert(float weight, const SVec3& xyz, const SVec3& tangent, const SVec3& binormal, const SVec3& normal)
		: weight(weight)
		, xyz(xyz)
		, tangent(tangent)
		, binormal(binormal)
		, normal(normal)
	{
	}
};
typedef vector<SBoneVert>	BoneVertVec;
//========================================================================

//========================================================================
// struct SBoneBucket
//========================================================================
struct SBoneBucket
{
	BoneVertVec		_influencedVerts;
	uivec			_indices;
	uint			_boneIndex;

	uint			GetNumVerts() const		{ return (uint)_influencedVerts.size(); }
};
typedef unordered_map<uint, SBoneBucket>		BoneBucketMap;
//========================================================================

//========================================================================
// struct GrSkin_impl
//========================================================================
struct GrSkin_impl
{
	BoneBucketMap		_boneBuckets;
	EVecRawMem<TriIdx>	_indices;

	SVec3vec			_normals;
	SVec3vec			_binormals;
	SVec3vec			_tangents;

	SVec3vec			_positions;
	SVec2vec			_uvs;

	GrSkin_impl()
		: _indices(256)
	{
	}
};
//========================================================================

#if 0
//===================
// GrSkin::RecomputeTangentsBinormals
//===================
void			GrSkin::RecomputeTangentsBinormals()
{
	E_ASSERT(_impl->_indices.size() % 3 == 0);
	uint numTris = (_impl->_indices.size() / 3);

	uint numVerts((uint)_impl->_positions.size());
	uivec averagesCount;
	averagesCount.resize(numVerts);

	_impl->_binormals.resize(numVerts);
	_impl->_tangents.resize(numVerts);
	BufZero(&_impl->_binormals[0], numVerts);
	BufZero(&_impl->_tangents[0], numVerts);

	for (uint triIdx = 0; triIdx < numTris; ++triIdx)
	{
		uint idxA(3*triIdx);
		uint idxB(3*triIdx + 1);
		uint idxC(3*triIdx + 2);
		uint idx[3] = { idxA, idxB, idxC };

		const SVec3& ptA(_impl->_positions[idxA]);
		const SVec3& ptB(_impl->_positions[idxB]);
		const SVec3& ptC(_impl->_positions[idxC]);

		const SVec2& tcA(_impl->_uvs[idxA]);
		const SVec2& tcB(_impl->_uvs[idxB]);
		const SVec2& tcC(_impl->_uvs[idxC]);

		SVec3 tangent, binormal;

		Gr_CalcTangents(tangent, binormal,
			ptA, ptB, ptC,
			tcA, tcB, tcC,
			false);

		for (int i = 0; i < 3; ++i)
		{
			uint vertIdx(idx[i]);
			_impl->_tangents[vertIdx] += tangent;
			_impl->_binormals[vertIdx] += binormal;
			++averagesCount[vertIdx];
		}
	}

	for (uint i = 0; i < numVerts; ++i)
		_impl->_tangents[i] *= (1.0f / averagesCount[i]);

	for (uint i = 0; i < numVerts; ++i)
		_impl->_binormals[i] *= (1.0f / averagesCount[i]);
}
#endif


//===================
// GrSkin::NormalizeTBN
//===================
void			GrSkin::NormalizeTBN()
{
	for (size_t i = 0; i < _impl->_tangents.size(); ++i)
		_impl->_tangents[i].Normalize();

	for (size_t i = 0; i < _impl->_binormals.size(); ++i)
		_impl->_binormals[i].Normalize();

	for (size_t i = 0; i < _impl->_normals.size(); ++i)
		_impl->_normals[i].Normalize();
}


//===================
// GrSkin::GrSkin
//===================
GrSkin::GrSkin(uint reserveVerts, uint reserveTris)
: _impl(E_NEW("skin", GrSkin_impl))
, _channels(0)
, _dirty(false)
{
	_impl->_tangents.reserve(reserveVerts);
	_impl->_binormals.reserve(reserveVerts);
	_impl->_normals.reserve(reserveVerts);

	_impl->_positions.reserve(reserveVerts);
	_impl->_uvs.reserve(reserveVerts);

	_impl->_indices.reserve(3 * reserveTris);
}


//===================
// GrSkin::~GrSkin
//===================
GrSkin::~GrSkin()
{
	E_DELETE("skin", _impl);
}


//===================
// GrSkin::GetNumVerts
//===================
uint			GrSkin::GetNumVerts() const
{
	return (uint)_impl->_positions.size();
}


//===================
// GrSkin::GetNumTris
//===================
uint			GrSkin::GetNumTris() const
{
	return (uint)(_impl->_indices.size() / 3);
}


//===================
// GrSkin::GetIndices
//===================
TriIdx*			GrSkin::GetIndices() const
{
	return _impl->_indices.ptr();
}


//===================
// GrSkin::GetTangents
//===================
SVec3*			GrSkin::GetTangents() const
{
	if (!HAS_FLAGS(_channels, MESH_TANGENTS))
		return NULL;
	return &_impl->_tangents[0];
}


//===================
// GrSkin::GetBinormals
//===================
SVec3*			GrSkin::GetBinormals() const
{
	if (!HAS_FLAGS(_channels, MESH_BINORMALS))
		return NULL;
	return &_impl->_binormals[0];
}


//===================
// GrSkin::GetNormals
//===================
SVec3*			GrSkin::GetNormals() const
{
	if (!HAS_FLAGS(_channels, MESH_NORMALS))
		return NULL;
	return &_impl->_normals[0];
}


//===================
// GrSkin::GetPositions
//===================
SVec3*			GrSkin::GetPositions() const
{
	return &_impl->_positions[0];
}


//===================
// GrSkin::GetTexcoords
//===================
SVec2*			GrSkin::GetTexcoords() const
{
	if (!HAS_FLAGS(_channels, MESH_TEXCOORDS))
		return NULL;
	return &_impl->_uvs[0];
}


//===================
// GrSkin::DeformVerts
//===================
SVec3*			GrSkin::DeformVerts(const MTransform** bones)
{
	if (!(_channels & MESH_BONE_INFO))
		return GetPositions();

	_dirty = true;

	uint numVerts(GetNumVerts());
	SVec3* positions(&_impl->_positions[0]);
	SVec3* tangents(NULL);
	SVec3* binormals(NULL);
	SVec3* normals(NULL);

	BufZero(positions, numVerts);

	if (_channels & MESH_TANGENTS)
	{
		tangents = &_impl->_tangents[0];
		BufZero(tangents, numVerts);
	}

	if (_channels & MESH_BINORMALS)
	{
		binormals = &_impl->_binormals[0];
		BufZero(binormals, numVerts);
	}

	if (_channels & MESH_NORMALS)
	{
		normals = &_impl->_normals[0];
		BufZero(normals, numVerts);
	}

	for (BoneBucketMap::iterator it(_impl->_boneBuckets.begin()), itEnd(_impl->_boneBuckets.end()); it != itEnd; ++it)
	{
		uint boneIdx(it->first);
		SBoneBucket& bucket(it->second);

		const MTransform& boneXform(*bones[boneIdx]);

		uint numBucketVerts(bucket.GetNumVerts());
		for (uint i = 0; i < numBucketVerts; ++i)
		{
			SBoneVert& vert(bucket._influencedVerts[i]);
			uint vertIdx(bucket._indices[i]);

			// deform position.
			{
				SVec3& deformedPos(positions[vertIdx]);
				SVec3 deformation(vert.xyz);
				boneXform.RotateTranslateXYZ(deformation.X(), deformation.Y(), deformation.Z());
				deformation *= vert.weight;
				deformedPos += deformation;
			}

			// deform tangent.
			if (tangents != NULL)
			{
				SVec3& deformedTangent(tangents[vertIdx]);
				SVec3 deformation(vert.tangent);
				boneXform.GetRot().RotatePointXYZ(deformation.X(), deformation.Y(), deformation.Z());
				deformation *= vert.weight;
				deformedTangent += deformation;
			}

			// deform binormal.
			if (binormals != NULL)
			{
				SVec3& deformedBinormal(binormals[vertIdx]);
				SVec3 deformation(vert.binormal);
				boneXform.GetRot().RotatePointXYZ(deformation.X(), deformation.Y(), deformation.Z());
				deformation *= vert.weight;
				deformedBinormal += deformation;
			}

			// deform normal.
			{
				SVec3& deformedNorm(normals[vertIdx]);
				SVec3 deformation(vert.normal);
				boneXform.GetRot().RotatePointXYZ(deformation.X(), deformation.Y(), deformation.Z());
				deformation *= vert.weight;
				deformedNorm += deformation;
			}
		}
	}

	NormalizeTBN();

	return GetPositions();
}


//===================
// GrSkin::Reset
//===================
void			GrSkin::Reset()
{
	_channels = 0;
	_impl->_boneBuckets.clear();
	_impl->_positions.clear();
	_impl->_uvs.clear();
	_impl->_indices.clear();
}


//===================
// GrSkin::StartVert
//===================
void			GrSkin::StartVert(const SVec3& pos, const SVec2& uv)
{
	_channels |= MESH_TEXCOORDS;
	_impl->_positions.push_back(pos);
	_impl->_uvs.push_back(uv);
	_curTotalWeight = 0.0f;
}


//===================
// GrSkin::AddNormals
//===================
void			GrSkin::AddNormals(const SVec3& normal)
{
	_channels |= MESH_NORMALS;
	SVec3 n(normal);
	n.Normalize();
	_impl->_normals.push_back(n);
}


//===================
// GrSkin::AddTangents
//===================
void			GrSkin::AddTangents(const SVec3& uBasis, const SVec3& vBasis)
{
	_channels |= (MESH_TANGENTS | MESH_BINORMALS);
	SVec3 u(uBasis);
	SVec3 v(vBasis);
	u.Normalize();
	v.Normalize();
	_impl->_tangents.push_back(u);
	_impl->_binormals.push_back(v);
}


//===================
// GrSkin::AddBoneWeight
//===================
void			GrSkin::AddBoneWeight(byte boneIndex, float weight)
{
	_channels |= MESH_BONE_INFO;

	E_ASSERT(boneIndex != byte(-1));
	E_ASSERT(weight > 0.0f);

	_curTotalWeight += weight;
	E_ASSERT(_curTotalWeight <= 1.001f);

	SBoneBucket& bucket(_impl->_boneBuckets[boneIndex]);
	SVec3& curXYZ(*_impl->_positions.rbegin());
	SVec3& curNormal(*_impl->_normals.rbegin());

	if (_impl->_tangents.size() == _impl->_positions.size() &&
		_impl->_binormals.size() == _impl->_positions.size())
	{
		SVec3& curTangent(*_impl->_tangents.rbegin());
		SVec3& curBinormal(*_impl->_binormals.rbegin());
		bucket._influencedVerts.push_back(SBoneVert(weight, curXYZ, curTangent, curBinormal, curNormal));
	}
	else
	{
		bucket._influencedVerts.push_back(SBoneVert(weight, curXYZ, curNormal));
	}

	uint curIdx((uint)(_impl->_positions.size() - 1));
	bucket._indices.push_back(curIdx);
}


//===================
// GrSkin::AddTriangle
//===================
void			GrSkin::AddTriangle(TriIdx idxA, TriIdx idxB, TriIdx idxC)
{
	_impl->_indices.push_back(idxA);
	_impl->_indices.push_back(idxB);
	_impl->_indices.push_back(idxC);
}


