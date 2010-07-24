//========================================================================
//	file:		import_k2_anim.cpp
//	author:		Shawn Presser 
//
// (c) 2010 Shawn Presser.  All Rights Reserved.
//========================================================================

//========================================================================
// Headers
//========================================================================
#include "e_common.h"
#include "import_k2_anim.h"

// engine headers.
#include "e_file.h"

// graphics headers.
#include "gr_driver.h"
#include "gr_anim.h"
#include "gr_anim_node.h"

// math headers.
#include "m_quat.h"
//========================================================================

//========================================================================
// Constants
//========================================================================
#define K2_FRAMES_PER_SECOND			24.0f
//========================================================================

//===================
// ImportK2Anim::ImportK2Anim
//===================
ImportK2Anim::ImportK2Anim(GrDriver& driver)
: _version(-1)
, _numMotions(-1)
, _numFrames(-1)
, _driver(driver)
, _file(NULL)
, _anim(NULL)
{
}


//===================
// ImportK2Anim::~ImportK2Anim
//===================
ImportK2Anim::~ImportK2Anim()
{
}


//===================
// ImportK2Anim::EulerToQuat
//===================
MQuat			ImportK2Anim::AnglesToQuat(float pitch, float roll, float yaw)
{
	pitch = DEGREES_TO_ANGLE(pitch);
	roll = DEGREES_TO_ANGLE(roll);
	yaw = DEGREES_TO_ANGLE(yaw);

	float sr, cr, sp, cp, sy, cy;

	SINCOS(roll * 0.5f, sr, cr);
	SINCOS(pitch * 0.5f, sp, cp);
	SINCOS(yaw * 0.5f, sy, cy);

	return MQuat(
		cy * cp * cr - sy * sp * sr,
		cy * sp * cr - sy * cp * sr,
		cy * cp * sr + sy * sp * cr,
		cy * sp * sr + sy * cp * cr);
}


//===================
// ImportK2Anim::ReadKeys
//===================
bool			ImportK2Anim::ReadKeys(float* dst, uint stride, EFile* file, uint count)
{
	if (count == 0)
		return true;

	E_VERIFY(dst != NULL && file != NULL, return false);
	E_VERIFY(stride > 0, return false);

	do 
	{
		*dst = file->ReadFloat();
		dst += stride;
	} while (--count > 0);

	return true;
}


//===================
// ImportK2Anim::ParseHeader
//===================
bool			ImportK2Anim::ParseHeader(GrDriver& driver, EFile* file, uint blockSize)
{
	_version = file->ReadInt();
	_numMotions = file->ReadInt();
	_numFrames = file->ReadInt();

	return true;
}


//===================
// ImportK2Anim::ParseBoneMotion
//===================
bool			ImportK2Anim::ParseBoneMotion(GrDriver& driver, EFile* file, uint blockSize)
{
	uint boneIndex = file->ReadInt();
	int type = file->ReadInt();
	uint numKeys = file->ReadInt();

	byte nameLen = file->ReadByte();
	char boneName[256];
	file->ReadArray(boneName, nameLen + 1);

	E_VERIFY(numKeys > 0, return false);

	GrAnimNode& animNode(_anim->GetTrack(StringToWString(boneName)));
	animNode.SetMaxKeyframes(_numFrames);
	animNode.SetNumKeyframes(MAX(numKeys, animNode.GetNumKeyframes()));
	animNode.SetAnimLength(_numFrames / K2_FRAMES_PER_SECOND);

	switch (type)
	{
	case 0: // X
		{
			uint& numXframes(animNode.GetNumPosXKeyframes());
			numXframes = MAX(numXframes, numKeys);
			ReadKeys(&animNode.GetPosKeyframes()->X(), 3, file, numKeys);
		}
		break;

	case 1: // Y
		{
			uint& numYframes(animNode.GetNumPosYKeyframes());
			numYframes = MAX(numYframes, numKeys);
			ReadKeys(&animNode.GetPosKeyframes()->Y(), 3, file, numKeys);
		}
		break;

	case 2: // Z
		{
			uint& numZframes(animNode.GetNumPosZKeyframes());
			numZframes = MAX(numZframes, numKeys);
			ReadKeys(&animNode.GetPosKeyframes()->Z(), 3, file, numKeys);
		}
		break;

	case 3: // Pitch
		{
			uint& numPitchFrames(animNode.GetUserData(0));
			numPitchFrames = MAX(numPitchFrames, numKeys);
			ReadKeys(&animNode.GetUserKeyframes()->X(), 3, file, numKeys);
		}
		break;

	case 4: // Roll
		{
			uint& numRollFrames(animNode.GetUserData(1));
			numRollFrames = MAX(numRollFrames, numKeys);
			ReadKeys(&animNode.GetUserKeyframes()->Y(), 3, file, numKeys);
		}
		break;

	case 5: // Yaw
		{
			uint& numYawFrames(animNode.GetUserData(2));
			numYawFrames = MAX(numYawFrames, numKeys);
			ReadKeys(&animNode.GetUserKeyframes()->Z(), 3, file, numKeys);
		}
		break;

	case 6: // Visibility
		//E_WARN("import_k2_anim", _T("not implemented"));
		break;

	case 7: // Scale X
		ReadKeys(&animNode.GetScaleKeyframes()->X(), 3, file, numKeys);
		break;

	case 8: // Scale Y
		ReadKeys(&animNode.GetScaleKeyframes()->Y(), 3, file, numKeys);
		break;

	case 9: // Scale Z
		ReadKeys(&animNode.GetScaleKeyframes()->Z(), 3, file, numKeys);
		break;
	}

	return true;
}


//===================
// ImportK2Anim::Read
//===================
GrAnim*			ImportK2Anim::Read(EFile* file)
{
	E_VERIFY(file != NULL && file->IsOpen(), return NULL);
	E_ASSERT(_anim == NULL);

	_file = file;
	_anim = E_NEW("import_k2_anim", GrAnim)(file->GetName());

	uint size(file->GetFileSize());

	// check the header.
	if (size < 4)
		return false;

	{
		byte sigC(file->ReadByte());
		byte sigL(file->ReadByte());
		byte sigI(file->ReadByte());
		byte sigP(file->ReadByte());

		if ((char)sigC != 'C' || (char)sigL != 'L' || (char)sigI != 'I' || (char)sigP != 'P')
			return false;
	}

	bool checkHeader(true);

	while (file->GetPos() + 4 + sizeof(int) < file->GetFileSize())
	{
		char blockName[5];
		for (int i = 0; i < 4; ++i)
			blockName[i] = (char)file->ReadByte();
		blockName[4] = 0;

		int blockSize = file->ReadInt();

		uint filePos(file->GetPos());

		if (checkHeader)
		{
			checkHeader = false;
			E_VERIFY(strncmp(blockName, "head", 4) == 0, return NULL);
			ParseHeader(_driver, _file, (uint)blockSize);
		}
		else if (strncmp(blockName, "bmtn", 4) == 0)
			ParseBoneMotion(_driver, _file, (uint)blockSize);

		file->Seek(filePos + blockSize);
	}
	file->Close();

	// build the rotations.
	for (uint i = 0, count = _anim->GetNumTracks(); i < count; ++i)
	{
		GrAnimNode& track(_anim->GetTrack(i));

		uint numPitchFrames(MAX(1, track.GetUserData(0)));
		uint numRollFrames(MAX(1, track.GetUserData(1)));
		uint numYawFrames(MAX(1, track.GetUserData(2)));

		for (uint frameIdx = 0, frameCount = track.GetNumKeyframes(); frameIdx < frameCount; ++frameIdx)
		{
			// rotation.
			MQuat& quat(track.GetRotKeyframe(frameIdx));
			float pitch = track.GetUserKeyframe(frameIdx % numPitchFrames).X();
			float roll = track.GetUserKeyframe(frameIdx % numRollFrames).Y();
			float yaw = track.GetUserKeyframe(frameIdx % numYawFrames).Z();
			quat = AnglesToQuat(pitch, roll, yaw);

			// position.
			MVec3& pos(track.GetPosKeyframe(frameIdx));
			uint x = MAX(1, track.GetNumPosXKeyframes());
			uint y = MAX(1, track.GetNumPosYKeyframes());
			uint z = MAX(1, track.GetNumPosZKeyframes());
			pos.X() = track.GetPosKeyframe(frameIdx % x).X();
			pos.Y() = track.GetPosKeyframe(frameIdx % y).Y();
			pos.Z() = track.GetPosKeyframe(frameIdx % z).Z();
		}
	}

	_anim->Finalize();

#if E_PRINT_DEBUG_INFO
	_anim->PrintDebug();
#endif

	GrAnim* result = _anim;
	_anim = NULL;
	_file = NULL;

	return result;
}


