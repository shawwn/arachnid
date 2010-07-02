//========================================================================
//	file:		gr_model.cpp
//	author:		Shawn Presser 
//	date:		7/2/10
//
// (c) 2010 Shawn Presser.  All Rights Reserved.
//========================================================================

//========================================================================
// Headers
//========================================================================
#include "e_common.h"
#include "gr_model.h"
//========================================================================

//===================
// GrModel::Sanitize
//===================
wstring			GrModel::Sanitize(const wstring& pathname)
{
	// case-insensitive and ignore leading/trailing whitespace
	wstring result(StrLower(StrTrim(pathname)));

	// convert backslashes to forward slashes.
	result = StrReplace(result, _T("\\"), _T("/"));

	// convert multiple consecutive forward slashes to a single forward slash.
	result = StrCompact(result, _T("/"), _T("/"), 2);

	// strip off leading forward slashes.
	while (!result.empty() && StrBeginsWith(result, _T("/")))
		result = result.substr(1);

	// strip off trailing forward slashes.
	while (!result.empty() && StrEndsWith(result, _T("/")))
		result = result.substr(0, result.size()-1);

	return result;
}


//===================
// GrModel::IsNameValid
//===================
bool				GrModel::IsNameValid(const wstring& sanitizedName)
{
	// must specify a name.
	if (sanitizedName.empty())
		return false;

	// verify that the sanitized name does not contain any special path characters.
	if (StrContains(sanitizedName, _T("/")))
		return false;
	if (StrContains(sanitizedName, _T("*")))
		return false;
	if (StrContains(sanitizedName, _T("\"")))
		return false;

	return true;
}


//===================
// GrModel::GrModel
//===================
GrModel::GrModel()
{
}


//===================
// GrModel::Create
//===================
GrModel*				GrModel::Create(const wstring& ctx, const wstring& dirtyName)
{
	// validate parameters.
	wstring name(Sanitize(dirtyName));
	if (!IsNameValid(name))
	{
		E_WARN("model", _TS("invalid model name '") + name + _T("'"));
		return NULL;
	}

	// create and return the result.
	GrModel* result(E_NEW(ctx, GrModel));
	result->_name = name;
	return result;
}


//===================
// GrModel::~GrModel
//===================
GrModel::~GrModel()
{
}


//===================
// GrModel::AddModel
//===================
bool					GrModel::AddModel(GrModel* model, const wstring& dirtyChildPath)
{
	wstring childPath(Sanitize(dirtyChildPath));
	if (!childPath.empty())
	{
		// extract the name of the child.  Example:
		//		"car/engine" => "car"
		wstring childPathRemainder;
		wstring childName(StrBefore(childPath, _T("/"), &childPathRemainder));

		// get the child.
		ModelMap::const_iterator itFind(_models.find(childName));
		if (itFind == _models.end())
			return false;
		GrModel* child(itFind->second);

		// add the model to the child.
		return child->AddModel(model, childPathRemainder);
	}

	wstring name(Sanitize(model->GetName()));
	if (!IsNameValid(name))
	{
		E_WARN("model", _TS("AddModel: invalid model name '") + name + _T("'"));
		return false;
	}

	// if the model already exists with the same name, return true.
	ModelMap::const_iterator itFind(_models.find(name));
	if (itFind != _models.end())
	{
		GrModel* existingModel(itFind->second);
		if (existingModel != model)
		{
			E_WARN("model", _TS("AddModel: tried to add a different model with the same name '") + name + _T("'"));
			return false;
		}
		return true;
	}

	_models.insert(make_pair(name, model));
	_modelsVec.push_back(model);
	return true;
}


//===================
// GrModel::HasModel
//===================
bool					GrModel::HasModel(const wstring& dirtyModelPath) const
{
	wstring modelPath(Sanitize(dirtyModelPath));
	if (StrContains(modelPath, _T("/")))
	{
		// extract the name of the child.  Example:
		//		"car/engine" => "car"
		wstring childPathRemainder;
		wstring childName(StrBefore(modelPath, _T("/"), &childPathRemainder));

		// get the child.
		ModelMap::const_iterator itFind(_models.find(childName));
		if (itFind == _models.end())
			return false;
		GrModel* child(itFind->second);

		// recurse.
		return child->HasModel(childPathRemainder);
	}

	// if we don't contain the model, fail.
	ModelMap::const_iterator itFind(_models.find(modelPath));
	if (itFind == _models.end())
		return false;

	return true;
}


//===================
// GrModel::GetModel
//===================
GrModel*				GrModel::GetModel(const wstring& dirtyModelPath) const
{
	wstring modelPath(Sanitize(dirtyModelPath));
	if (StrContains(modelPath, _T("/")))
	{
		// extract the name of the child.  Example:
		//		"car/engine" => "car"
		wstring childPathRemainder;
		wstring childName(StrBefore(modelPath, _T("/"), &childPathRemainder));

		// get the child.
		ModelMap::const_iterator itFind(_models.find(childName));
		if (itFind == _models.end())
			return NULL;
		GrModel* child(itFind->second);

		// recurse.
		return child->GetModel(childPathRemainder);
	}


	// if we don't contain the model, fail.
	ModelMap::const_iterator itFind(_models.find(modelPath));
	if (itFind == _models.end())
		return NULL;
	GrModel* model(itFind->second);

	return model;
}

