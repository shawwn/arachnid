//========================================================================
//	file:		gr_anim.h
//	author:		Shawn Presser 
//	date:		7/12/10
//
// (c) 2010 Shawn Presser.  All Rights Reserved.
//========================================================================
#pragma once

//========================================================================
// Declarations
//========================================================================
class GrAnimNode;
struct GrAnim_impl;
//========================================================================

//========================================================================
// GrAnim
//========================================================================
class ENGINE_API GrAnim
{
private:
	wstring			_name;
	GrAnim_impl*	_impl;
	uint			_maxNumFrames;

public:
	GrAnim(const wstring& name);
	~GrAnim();

	void			PrintDebug();

	void			SetName(const wstring& name)	{ _name = name; }
	const wstring&	GetName() const					{ return _name; }

	uint			GetMaxNumKeyframes() const		{ return _maxNumFrames; }

	void			Reset();

	// provides array access to the tracks.
	uint			GetNumTracks() const;
	GrAnimNode&		GetTrack(uint idx);

	// creates or gets a track with the specified name.
	GrAnimNode&		GetTrack(const wstring& name);

	// tries to find a track with the specified name.
	GrAnimNode*		FindTrack(const wstring& name) const;

	// each time you have modified the animation, call Finalize.
	void			Finalize();

	void			Update(uint time);
};
//========================================================================

