//========================================================================
//	file:		import_k2_anim.h
//	author:		Shawn Presser 
//	date:		7/7/10
//
// (c) 2010 Shawn Presser.  All Rights Reserved.
//========================================================================
#pragma once

//========================================================================
// Declarations
//========================================================================
class EFile;
class GrDriver;
class GrAnim;
class MQuat;
class MVec3;
//========================================================================

//========================================================================
// ImportK2Anim
//========================================================================
class ENGINE_API ImportK2Anim
{
private:
	int					_version;
	int					_numMotions;
	int					_numFrames;
	GrDriver&			_driver;
	EFile*				_file;

	GrAnim*				_anim;

	static MQuat		AnglesToQuat(float pitch, float roll, float yaw);

	bool				ReadKeys(float* dst, uint stride, EFile* file, uint count);

	bool				ParseHeader(GrDriver& driver, EFile* file, uint blockSize);
	bool				ParseBoneMotion(GrDriver& driver, EFile* file, uint blockSize);
public:
	ImportK2Anim(GrDriver& driver);
	~ImportK2Anim();

	GrAnim*				Read(EFile* file);
};
//========================================================================





