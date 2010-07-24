//========================================================================
//	file:		gr_anim_node.cpp
//	author:		Shawn Presser 
//
// (c) 2010 Shawn Presser.  All Rights Reserved.
//========================================================================

//========================================================================
// Headers
//========================================================================
#include "e_common.h"
#include "gr_anim_node.h"

// graphics headers.
#include "gr_model_node.h"

// math headers.
#include "m_mat44.h"
#include "m_mat33.h"
#include "m_quat.h"
#include "m_vec3.h"
//========================================================================

//===================
// KeyframePosition
//===================
static bool			KeyframePosition(uint& outFrame, float& outT, uint numFrames, float animLength, float elapsedTime)
{
	if (numFrames == 1)
	{
		outFrame = 0;
		outT = (elapsedTime / animLength);
		return true;
	}

	E_ASSERT(numFrames > 1);
	E_VERIFY(animLength >= 0.0f && elapsedTime < animLength, return false);

	float pos = (numFrames - 1) * (elapsedTime / animLength);
	outFrame = FLOOR(pos);
	outT = FRAC(pos);
	return true;
}


//===================
// GrAnimNode::GrAnimNode
//===================
GrAnimNode::GrAnimNode(const wstring& nodeName)
: _maxKeyframes(0)
, _numKeyframes(0)
, _animLength(0.0f)
, _name(nodeName)
{
	_numPosXKeyframes = 0;
	_numPosYKeyframes = 0;
	_numPosZKeyframes = 0;

	ArrayZero(_userdata, 3);
	_userKeyframes = NULL;
	_posKeyframes = NULL;
	_scaleKeyframes = NULL;
	_rotKeyframes = NULL;
}


//===================
// GrAnimNode::~GrAnimNode
//===================
GrAnimNode::~GrAnimNode()
{
	Reset();
}


//===================
// GrAnimNode::Reset
//===================
void			GrAnimNode::Reset()
{
	ArrayZero(_userdata, 3);
	E_DELETE_ARRAY("animnode", _userKeyframes);
	E_DELETE_ARRAY("animnode", _posKeyframes);
	E_DELETE_ARRAY("animnode", _scaleKeyframes);
	E_DELETE_ARRAY("animnode", _rotKeyframes);
	_maxKeyframes = 0;
	_numKeyframes = 0;
}


//===================
// GrAnimNode::Reset
//===================
void			GrAnimNode::Reset(uint count, float animLength)
{
	Reset();
	E_VERIFY(count > 0 && animLength > 0.0f, return);
	SetMaxKeyframes(count);
	SetNumKeyframes(count);
	SetAnimLength(animLength);
}


//===================
// GrAnimNode::SetMaxKeyframes
//===================
void			GrAnimNode::SetMaxKeyframes(uint maxKeyframes)
{
	E_VERIFY(maxKeyframes > 0, return);

	if (maxKeyframes == _maxKeyframes)
		return;

	Reset();

	_maxKeyframes = maxKeyframes;
	_numKeyframes = MIN(_numKeyframes, maxKeyframes);

	_userKeyframes = E_NEW_ARRAY("animnode", MVec3, maxKeyframes);
	_posKeyframes = E_NEW_ARRAY("animnode", MVec3, maxKeyframes);
	_scaleKeyframes = E_NEW_ARRAY("animnode", MVec3, maxKeyframes);
	_rotKeyframes = E_NEW_ARRAY("animnode", MQuat, maxKeyframes);

	ArrayZero(_userKeyframes, maxKeyframes);
	ArrayZero(_posKeyframes, maxKeyframes);
	ArrayZero(_scaleKeyframes, maxKeyframes);
}


//===================
// GrAnimNode::AnimateNode
//===================
void			GrAnimNode::AnimateNode(GrModelNode* node, float elapsedTime) const
{
	// validate preconditions.
	E_VERIFY(elapsedTime >= 0.0f && elapsedTime < _animLength, return);
	E_VERIFY(_maxKeyframes > 0 && _numKeyframes > 0, return);

	// sanity check.
	E_ASSERT(!FloatZero(_animLength));

	// determine the two keyframes to blend between.
	uint frame;
	float t;
	if (!KeyframePosition(frame, t, _maxKeyframes, _animLength, elapsedTime))
		return;

	frame = MIN(frame, _numKeyframes - 1);

	uint frameNext = frame + 1;
	if (frameNext > _numKeyframes - 1)
	{
		frameNext = MIN(frameNext, _numKeyframes - 1);
		t = 0.0f;
	}

	E_ASSERT(frame < _numKeyframes && frameNext < _numKeyframes);
	E_ASSERT(frame <= frameNext);

	const MQuat& frameArot(_rotKeyframes[frame]);
	const MVec3& frameApos(_posKeyframes[frame]);
	const MQuat& frameBrot(_rotKeyframes[frameNext]);
	const MVec3& frameBpos(_posKeyframes[frameNext]);

	MTransform blended;
	if (frame == frameNext)
	{
		blended.SetRot(frameArot.ToRotationMatrix());
		blended.SetTrans(frameApos);
	}
	else
	{
		E_ASSERT(BETWEEN(0.0f, 1.0f, t));
		blended.SetRot(frameArot.Blend(frameBrot, t).ToRotationMatrix());
		blended.SetTrans(frameApos.Blend(frameBpos, t));
	}

	node->SetLocal(blended);
}


