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
//========================================================================

//========================================================================
// GrModel
//========================================================================
class GrModel
{
private:
	wstring			_name;

	typedef hash_map<wstring, GrModel*>		ModelMap;
	typedef vector<GrModel*>				ModelVec;

	ModelMap		_models;
	ModelVec		_modelsVec;

	static wstring			Sanitize(const wstring& pathname);
	static bool				IsNameValid(const wstring& sanitizedName);

	GrModel();
public:
	static GrModel*			Create(const wstring& ctx, const wstring& modelName);
	~GrModel();

	const wstring&			GetName() const				{ return _name; }

	// adds a model to the tree.
	// * returns true if the model is already in the tree.
	// * returns false if the model has an invalid name.
	// * 'childPath' allows you to deep add a model.  For example,
	//		AddModel(helmet, "car/driver") will add the helmet to the
	//		driver of the car.
	bool					AddModel(GrModel* model, const wstring& childPath = _T(""));

	// gets a model in the tree.
	//		GetModel("car/driver") will return true if we contain the
	//		model 'car' which contains the model 'driver'.
	bool					HasModel(const wstring& modelPath) const;
	GrModel*				GetModel(const wstring& modelPath) const;

	// provides array access to the child models.
	GrModel*				GetModel(uint idx)			{ E_VERIFY(idx < _modelsVec.size(), return NULL); return _modelsVec[idx]; }
	uint					NumModels() const			{ return _modelsVec.size(); }
};
//========================================================================


