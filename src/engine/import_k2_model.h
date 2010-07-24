//========================================================================
//	file:		import_k2_model.h
//	author:		Shawn Presser 
//	date:		7/7/10
//
// (c) 2010 Shawn Presser.  All Rights Reserved.
//========================================================================
#pragma once

//========================================================================
// Headers
//========================================================================
#include "m_vec3.h"
//========================================================================

//========================================================================
// Declarations
//========================================================================
class EFile;
class GrDriver;
class GrModel;
class GrMaterial;
class GrSkin;
class MMat33;
struct ImportK2Model_stl;
//========================================================================

//========================================================================
// ImportK2Model
//========================================================================
class ENGINE_API ImportK2Model
{
private:
	ImportK2Model_stl*	_stl;

	GrDriver&			_driver;
	EFile*				_file;

	GrModel*			_model;
	GrMaterial*			_nullMat;

	int					_numMeshes;
	int					_numSprites;
	int					_numSurfaces;
	int					_numBones;
	MVec3				_bbMin;
	MVec3				_bbMax;

	bool				ParseHeader(GrDriver& driver, EFile* file, int blockSize);
	bool				ParseMesh(GrDriver& driver, EFile* file, int blockSize);
	bool				ParseVertices(GrDriver& driver, EFile* file, int blockSize);
	bool				ParseNormals(GrDriver& driver, EFile* file, int blockSize);
	bool				ParseTangents(GrDriver& driver, EFile* file, int blockSize);
	bool				ParseTexcoords(GrDriver& driver, EFile* file, int blockSize);
	bool				ParseTriangles(GrDriver& driver, EFile* file, int blockSize);
	bool				ParseBones(GrDriver& driver, EFile* file, int blockSize);
	bool				ParseSkin(GrDriver& driver, EFile* file, int blockSize);
public:
	ImportK2Model(GrDriver& driver, GrMaterial* nullMat);
	~ImportK2Model();

	bool				Read(EFile* file);

	GrModel*			GetModel()				{ return _model; }
};
//========================================================================




