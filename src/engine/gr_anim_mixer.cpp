//========================================================================
//	file:		gr_anim_mixer.cpp
//	author:		Shawn Presser 
//
// (c) 2010 Shawn Presser.  All Rights Reserved.
//========================================================================

//========================================================================
// Headers
//========================================================================
#include "e_common.h"
#include "gr_anim_mixer.h"

// graphics headers.
#include "gr_model_node.h"
#include "gr_anim_node.h"
#include "gr_anim.h"
//========================================================================


//===================
// GrAnimMixer::AnimateNode
//===================
void			GrAnimMixer::AnimateNode(GrAnim* anim, GrModelNode* node, bool recurse)
{
	const wstring& trackName(node->GetName());
	GrAnimNode* animNode(anim->FindTrack(trackName));
	if (animNode != NULL)
	{
		float time = fmod(0.0125f*_elapsedTime / 1000.0f, animNode->GetAnimLength());
		animNode->AnimateNode(node, time);
	}

	if (recurse)
	{
		// recurse to children.
		uint numChildren = node->GetNumChildModelNodes();
		for (uint i = 0; i < numChildren; ++i)
		{
			GrModelNode* child(node->GetChildModelNode(i));
			AnimateNode(anim, child);
		}
	}
}


//===================
// GrAnimMixer::GrAnimMixer
//===================
GrAnimMixer::GrAnimMixer(GrModelNode* root)
: _root(root)
, _anim(NULL)
, _elapsedTime(0)
{
}


//===================
// GrAnimMixer::~GrAnimMixer
//===================
GrAnimMixer::~GrAnimMixer()
{
}


//===================
// GrAnimMixer::PlayAnim
//===================
void			GrAnimMixer::PlayAnim(GrAnim* anim)
{
	_anim = anim;
}


//===================
// GrAnimMixer::Update
//===================
void			GrAnimMixer::Update(uint deltaTime)
{
	if (deltaTime == 0)
		return;

	_elapsedTime += deltaTime;

	// reset each bone.
#if 1
	_root->ResetHierarchyToLocal(MTransform());

	if (_anim != NULL)
		AnimateNode(_anim, _root);
#else
	_root->ResetHierarchyToBindPose();
	_root->Find(_T("Bip01"))->RotateHierarchy(MMat33::FromXRot(PERCENT_TO_ANGLE(-25.0f)));
	_root->Find(_T("Bip01 Spine2"))->RotateHierarchy(MMat33::FromXRot(PERCENT_TO_ANGLE(25.0f)));
	//_root->Find(_T("Bip01 Pelvis"))->TransformHierarchy(MTransform(MMat33::FromAngles(PERCENT_TO_ANGLE( 25.0f), 0.0f, 0.0f)));
#endif

}
