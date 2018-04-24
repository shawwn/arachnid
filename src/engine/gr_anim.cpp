//========================================================================
//	file:		gr_anim.cpp
//	author:		Shawn Presser 
//
// (c) 2010 Shawn Presser.  All Rights Reserved.
//========================================================================

//========================================================================
// Headers
//========================================================================
#include "e_common.h"
#include "gr_anim.h"

// graphics headers.
#include "gr_anim_node.h"

// debug headers.
#include "e_filemanager.h"
#include "m_mat33.h"
#include "m_transform.h"
//========================================================================

//========================================================================
// GrAnim_impl
//========================================================================
typedef unordered_map<wstring, GrAnimNode>		AnimTrackMap;
typedef map<wstring, GrAnimNode*>			AnimTrackSortedMap;
typedef vector<GrAnimNode*>					AnimTrackVec;
struct GrAnim_impl
{
	AnimTrackMap		tracks;
	AnimTrackVec		tracksVec;
};
//========================================================================

//========================================================================
// GrAnim
//========================================================================

//===================
// GrAnim::GrAnim
//===================
GrAnim::GrAnim(const wstring& name)
: _name(name)
, _maxNumFrames(0)
{
	_impl = E_NEW("anim", GrAnim_impl);
}


//===================
// GrAnim::~GrAnim
//===================
GrAnim::~GrAnim()
{
	E_DELETE("anim", _impl);
}


//===================
// GrAnim::Reset
//===================
void			GrAnim::Reset()
{
	_impl->tracks.clear();
	_impl->tracksVec.clear();
	_maxNumFrames = 0;
}


//===================
// GrAnim::PrintDebug
//===================
void			GrAnim::PrintDebug()
{
	// print the animation.
	wstring animFilePath(PRINT_STR(_T("~anim-%s.txt"), _name.c_str()));
	EFile* animFile = gFileManager->GetFile(animFilePath, FILE_WRITE | FILE_TEXT);

	AnimTrackSortedMap sortedMap;
	for (AnimTrackMap::iterator it(_impl->tracks.begin()), itEnd(_impl->tracks.end()); it != itEnd; ++it)
	{
		GrAnimNode& animNode(it->second);
		sortedMap[animNode.GetName()] = &animNode;
	}

	// print each anim track.
	for (AnimTrackSortedMap::iterator it(sortedMap.begin()), itEnd(sortedMap.end()); it != itEnd; ++it)
	{
		GrAnimNode& animNode(*it->second);
		uint numKeyframes(animNode.GetNumKeyframes());

		wstring filePath(PRINT_STR(_T("~anim-%s-%s.txt"), _name.c_str(), animNode.GetName().c_str()));
		EFile* animTrackFile = gFileManager->GetFile(filePath, FILE_WRITE | FILE_TEXT);

		// print header.

		if (animFile != NULL)
		{
			animFile->BeginTextHeader();
			animFile->WriteLine(_TP("Anim Track [%s]", animNode.GetName().c_str()));
			animFile->WriteLine(_TP("%u keyframe(s)", numKeyframes));
			animFile->EndTextHeader();
		}

		if (animTrackFile != NULL)
		{
			animTrackFile->BeginTextHeader();
			animTrackFile->WriteLine(_TP("Anim Track [%s]", animNode.GetName().c_str()));
			animTrackFile->WriteLine(_TP("%u keyframe(s)", numKeyframes));
			animTrackFile->EndTextHeader();
		}

		// print each keyframe.
		for (uint i = 0; i < numKeyframes; ++i)
		{
			// get frame data.
			MQuat& rot(animNode.GetRotKeyframe(i));
			MVec3& pos(animNode.GetPosKeyframe(i));
			
			// convert the frame to a transform.
			MTransform trans(rot.ToRotationMatrix(), pos);

			// print the frame.
			wstring strFrame(PRINT_STR(_T("frame [%d]:\t%s"), i, TO_STR(trans).c_str()));

			if (animTrackFile != NULL)
				animTrackFile->WriteLine(strFrame);

			if (animFile != NULL)
				animFile->WriteLine(strFrame);
		}
		E_DELETE("anim", animTrackFile);

		if (animFile != NULL)
			animFile->WriteBlankLine();
	}
	E_DELETE("anim", animFile);
}


//===================
// GrAnim::GetNumTracks
//===================
uint			GrAnim::GetNumTracks() const
{
	return (uint)_impl->tracks.size();
}


//===================
// GrAnim::GetTrack
//===================
GrAnimNode&		GrAnim::GetTrack(uint idx)
{
	E_ASSERT(idx < GetNumTracks());
	return *_impl->tracksVec[idx];
}


//===================
// GrAnim::GetTrack
//===================
GrAnimNode&		GrAnim::GetTrack(const wstring& name)
{
	AnimTrackMap::iterator itFind(_impl->tracks.find(name));
	if (itFind == _impl->tracks.end())
	{
		itFind = _impl->tracks.insert(std::make_pair(name, GrAnimNode(name))).first;
		_impl->tracksVec.push_back(&itFind->second);
	}

	GrAnimNode& animNode(itFind->second);
	return animNode;
}


//===================
// GrAnim::FindTrack
//===================
GrAnimNode*		GrAnim::FindTrack(const wstring& name) const
{
	AnimTrackMap::iterator itFind(_impl->tracks.find(name));
	if (itFind == _impl->tracks.end())
		return NULL;

	return &itFind->second;
}


//===================
// GrAnim::Finalize
//===================
void			GrAnim::Finalize()
{
	for (AnimTrackMap::iterator it(_impl->tracks.begin()), itEnd(_impl->tracks.end()); it != itEnd; ++it)
	{
		GrAnimNode& animNode(it->second);
		//animNode.Finalize();
		_maxNumFrames = MAX(_maxNumFrames, animNode.GetNumKeyframes());
	}
}


//===================
// GrAnim::Update
//===================
void			GrAnim::Update(uint time)
{
}
//========================================================================
