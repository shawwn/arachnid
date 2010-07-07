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
class GrDriver;
class EFile;
struct ImportK2Anim_impl;
//========================================================================

//========================================================================
// ImportK2Anim
//========================================================================
class ENGINE_API ImportK2Anim
{
private:
	ImportK2Anim_impl*	_impl;

	GrDriver&			_driver;
	EFile*				_file;

	bool				ParseHeader(GrDriver& driver, EFile* file, uint blockSize);
	bool				ParseBoneMotion(GrDriver& driver, EFile* file, uint blockSize);
public:
	ImportK2Anim(GrDriver& driver);
	~ImportK2Anim();

	bool				OnFinishReading(GrDriver& driver);

	bool				Read(EFile* file);
};
//========================================================================





