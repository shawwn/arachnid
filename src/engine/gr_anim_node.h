//========================================================================
//	file:		gr_anim_node.h
//	author:		Shawn Presser 
//	date:		7/2/10
//
// (c) 2010 Shawn Presser.  All Rights Reserved.
//========================================================================
#pragma once

//========================================================================
// Headers
//========================================================================
#include "m_vec3.h"
#include "m_quat.h"
//========================================================================

//========================================================================
// Declarations
//========================================================================
class GrModelNode;
class MQuat;
//========================================================================

//========================================================================
// GrAnimNode
//========================================================================
class ENGINE_API GrAnimNode
{
	friend class GrAnim;
private:
	wstring			_name;
	uint			_maxKeyframes;
	uint			_numKeyframes;
	float			_animLength;

	uint			_numPosXKeyframes;
	uint			_numPosYKeyframes;
	uint			_numPosZKeyframes;

	uint			_userdata[3];

	MVec3*			_userKeyframes;
	MVec3*			_posKeyframes;
	MVec3*			_scaleKeyframes;
	MQuat*			_rotKeyframes;

public:
	GrAnimNode(const wstring& nodeName);
	~GrAnimNode();

	void			SetName(const wstring& name)	{ _name = name; }
	const wstring&	GetName() const					{ return _name; }
	bool			IsNameEmpty() const				{ return _name.empty(); }

	MVec3*			GetUserKeyframes() const		{ return _userKeyframes; }
	MVec3*			GetPosKeyframes() const			{ return _posKeyframes; }
	MVec3*			GetScaleKeyframes() const		{ return _scaleKeyframes; }
	MQuat*			GetRotKeyframes() const			{ return _rotKeyframes; }

	uint&			GetNumPosXKeyframes()			{ return _numPosXKeyframes; }
	uint&			GetNumPosYKeyframes()			{ return _numPosXKeyframes; }
	uint&			GetNumPosZKeyframes()			{ return _numPosXKeyframes; }

	uint&			GetUserData(uint i)				{ E_ASSERT(i < 3); return _userdata[i]; }
	MVec3&			GetUserKeyframe(uint i)			{ E_ASSERT(i < _maxKeyframes); return _userKeyframes[i]; }
	MVec3&			GetPosKeyframe(uint i)			{ E_ASSERT(i < _maxKeyframes); return _posKeyframes[i]; }
	MVec3&			GetScaleKeyframe(uint i)		{ E_ASSERT(i < _maxKeyframes); return _scaleKeyframes[i]; }
	MQuat&			GetRotKeyframe(uint i)			{ E_ASSERT(i < _maxKeyframes); return _rotKeyframes[i]; }

	void			Reset();
	void			Reset(uint count, float animLength = 1.0f);

	void			SetMaxKeyframes(uint maxKeyframes);
	uint			GetMaxKeyframes() const			{ return _maxKeyframes; }

	void			SetNumKeyframes(uint numKeyframes)	{ _numKeyframes = numKeyframes; }
	uint			GetNumKeyframes() const			{ return _numKeyframes; }

	void			SetAnimLength(float len)		{ _animLength = MAX(0.0f, len); }
	float			GetAnimLength() const			{ return _animLength; }

	void			AnimateNode(GrModelNode* node, float elapsedTime) const;
};
//========================================================================

