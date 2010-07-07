//========================================================================
//	file:		import_k2_model.h
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
class GrModel;
class EFile;
class GrMaterial;
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

	bool				ParseMesh(GrDriver& driver, EFile* file, uint blockSize);
	bool				ParseVertices(GrDriver& driver, EFile* file, uint blockSize);
	bool				ParseTexcoords(GrDriver& driver, EFile* file, uint blockSize);
	bool				ParseTriangles(GrDriver& driver, EFile* file, uint blockSize);
public:
	ImportK2Model(GrDriver& driver, GrMaterial* nullMat);
	~ImportK2Model();

	bool				OnFinishReading(GrDriver& driver);

	bool				Read(EFile* file);

	GrModel*			GetModel()				{ return _model; }
};
//========================================================================




