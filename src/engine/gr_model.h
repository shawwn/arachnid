//========================================================================
//	file:		gr_model.h
//	author:		Shawn Presser 
//	date:		7/2/10
//
// (c) 2010 Shawn Presser.  All Rights Reserved.
//========================================================================
#pragma once

//========================================================================
// Declarations
//========================================================================
class GrModel;
class GrModelNode;
class GrAnim;
class GrAnimMixer;
class MTransform;
class EFile;
struct GrModel_stl;
//========================================================================

//========================================================================
// GrModel
//========================================================================
class ENGINE_API GrModel
{
private:
	wstring					_name;
	GrModel_stl*			_stl;
	GrModelNode*			_root;
	GrAnimMixer*			_animMixer;
	bool					_dirty;

	static wstring			Sanitize(const wstring& pathName);
	static bool				IsNameValid(const wstring& sanitizedName);

	void					DoPrintDebug(EFile* file, const wstring& modelPath = _T(""), uint indent = 0) const;

	GrModel();
public:
	static GrModel*			Create(const wchar_t* ctx, const wstring& modelName);
	~GrModel();

	void					PrintDebug(EFile* file = NULL, const wstring& donotset = _T(""), uint indent = 0) const;

	const wstring&			GetName() const				{ return _name; }

	// provides access to the model's skeleton.
	GrModelNode&			GetRoot()					{ return *_root; }
	const GrModelNode&		GetRoot() const				{ return *_root; }
	
	void					SetDirty()					{ _dirty = true; }
	bool					IsDirty() const				{ return _dirty; }

	// adds a model to the tree.
	// * returns true if the model is already in the tree.
	// * returns false if the model has an invalid name.
	// * 'childPath' allows you to deep add a model.  For example,
	//		AddChildModel(helmet, "car/driver") will add the helmet to the
	//		driver of the car.
	bool					AddChildModel(GrModel* model, const wstring& childPath = _T(""));

	// gets a model in the tree.
	//		GetChildModel("car/driver") will return true if we contain the
	//		model 'car' which contains the model 'driver'.
	bool					HasChildModel(const wstring& modelPath) const;
	GrModel*				GetChildModel(const wstring& modelPath) const;

	// provides array access to the child models.
	GrModel*				GetChildModel(uint idx) const;
	uint					GetNumChildModels() const;

	// animation controls.
	GrAnimMixer*			Animations();

	// updates the model hierarchy (plays animations, etc)
	void					Update(uint deltaTime);

	// ensures that the transforms of the skeleton are up-to-date.
	void					UpdateTransforms();
};
//========================================================================


