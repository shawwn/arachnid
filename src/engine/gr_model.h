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
class MMat44;
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

	static wstring			Sanitize(const wstring& pathName);
	static bool				IsNameValid(const wstring& sanitizedName);

	GrModel();
public:
	static GrModel*			Create(const wchar_t* ctx, const wstring& modelName);
	~GrModel();

	const wstring&			GetName() const				{ return _name; }

	// provides access to the model's skeleton.
	GrModelNode&			GetRoot()					{ return *_root; }
	const GrModelNode&		GetRoot() const				{ return *_root; }

	// the model's transform.
	const MMat44&			GetTransform() const;
	void					SetTransform(const MMat44& m);

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
	uint					NumChildModels() const;
};
//========================================================================


