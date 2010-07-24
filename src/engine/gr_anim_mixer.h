//========================================================================
//	file:		gr_anim_mixer.h
//	author:		Shawn Presser 
//	date:		7/13/10
//
// (c) 2010 Shawn Presser.  All Rights Reserved.
//========================================================================
#pragma once

//========================================================================
// Declarations
//========================================================================
class GrModelNode;
class GrAnim;
//========================================================================

//========================================================================
// GrAnimMixer
//========================================================================
class ENGINE_API GrAnimMixer
{
private:
	GrModelNode*	_root;

	// only one animation for now.
	GrAnim*			_anim;

	uint			_elapsedTime;

	void			AnimateNode(GrAnim* anim, GrModelNode* node, bool recurse = true);

public:
	GrAnimMixer(GrModelNode* root);
	~GrAnimMixer();

	GrAnim*			GetCurrentAnim() const			{ return _anim; }

	void			PlayAnim(GrAnim* anim);
	void			Update(uint deltaTime);
};
//========================================================================

