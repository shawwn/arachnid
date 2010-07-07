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
#include "gr_model_node.h"
//========================================================================

//========================================================================
// Declarations
//========================================================================
typedef hash_map<wstring, GrModel*>		ModelMap;
typedef vector<GrModel*>				ModelVec;
//========================================================================


//========================================================================
// GrModel_stl
//========================================================================
struct GrModel_stl
{
	ModelMap				models;
	ModelVec				modelsVec;
};
//========================================================================


//===================
// GrModel::Sanitize
//===================
wstring			GrModel::Sanitize(const wstring& pathName)
{
	// case-insensitive and ignore leading/trailing whitespace
	wstring result(StrLower(StrTrim(pathName)));

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
: _root(E_NEW("model", GrModelNode)(_T("root")))
, _stl(E_NEW("model", GrModel_stl))
{
}


//===================
// GrModel::Create
//===================
GrModel*				GrModel::Create(const wchar_t* ctx, const wstring& dirtyName)
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
	E_DELETE("model", _root);
	E_DELETE("model", _stl);
}


//===================
// GrModel::GetTransform
//===================
const MMat44&			GrModel::GetTransform() const
{
	return _root->GetTransform();
}


//===================
// GrModel::SetTransform
//===================
void					GrModel::SetTransform(const MMat44& m)
{
	_root->SetTransform(m);
}


//===================
// GrModel::AddChildModel
//===================
bool					GrModel::AddChildModel(GrModel* model, const wstring& dirtyChildPath)
{
	if (model == NULL)
		return false;

	wstring childPath(Sanitize(dirtyChildPath));
	if (!childPath.empty())
	{
		// extract the name of the child.  Example:
		//		"car/engine" => "car"
		wstring childPathRemainder;
		wstring childName(StrBefore(childPath, _T("/"), &childPathRemainder));

		// get the child.
		ModelMap::const_iterator itFind(_stl->models.find(childName));
		if (itFind == _stl->models.end())
			return false;
		GrModel* child(itFind->second);

		// add the model to the child.
		return child->AddChildModel(model, childPathRemainder);
	}

	wstring name(Sanitize(model->GetName()));
	if (!IsNameValid(name))
	{
		E_WARN("model", _TS("AddChildModel: invalid model name '") + name + _T("'"));
		return false;
	}

	// if the model already exists with the same name, return true.
	ModelMap::const_iterator itFind(_stl->models.find(name));
	if (itFind != _stl->models.end())
	{
		GrModel* existingModel(itFind->second);
		if (existingModel != model)
		{
			E_WARN("model", _TS("AddChildModel: tried to add a different model with the same name '") + name + _T("'"));
			return false;
		}
		return true;
	}

	_stl->models.insert(make_pair(name, model));
	_stl->modelsVec.push_back(model);
	return true;
}


//===================
// GrModel::HasChildModel
//===================
bool					GrModel::HasChildModel(const wstring& dirtyModelPath) const
{
	wstring modelPath(Sanitize(dirtyModelPath));
	if (StrContains(modelPath, _T("/")))
	{
		// extract the name of the child.  Example:
		//		"car/engine" => "car"
		wstring childPathRemainder;
		wstring childName(StrBefore(modelPath, _T("/"), &childPathRemainder));

		// get the child.
		ModelMap::const_iterator itFind(_stl->models.find(childName));
		if (itFind == _stl->models.end())
			return false;
		GrModel* child(itFind->second);

		// recurse.
		return child->HasChildModel(childPathRemainder);
	}

	// if we don't contain the model, fail.
	ModelMap::const_iterator itFind(_stl->models.find(modelPath));
	if (itFind == _stl->models.end())
		return false;

	return true;
}


//===================
// GrModel::GetChildModel
//===================
GrModel*				GrModel::GetChildModel(const wstring& dirtyModelPath) const
{
	wstring modelPath(Sanitize(dirtyModelPath));
	if (StrContains(modelPath, _T("/")))
	{
		// extract the name of the child.  Example:
		//		"car/engine" => "car"
		wstring childPathRemainder;
		wstring childName(StrBefore(modelPath, _T("/"), &childPathRemainder));

		// get the child.
		ModelMap::const_iterator itFind(_stl->models.find(childName));
		if (itFind == _stl->models.end())
			return NULL;
		GrModel* child(itFind->second);

		// recurse.
		return child->GetChildModel(childPathRemainder);
	}


	// if we don't contain the model, fail.
	ModelMap::const_iterator itFind(_stl->models.find(modelPath));
	if (itFind == _stl->models.end())
		return NULL;
	GrModel* model(itFind->second);

	return model;
}


//===================
// GrModel::GetChildModel
//===================
GrModel*				GrModel::GetChildModel(uint idx) const
{
	E_VERIFY(idx < _stl->modelsVec.size(), return NULL);
	return _stl->modelsVec[idx];
}


//===================
// GrModel::NumChildModels
//===================
uint					GrModel::NumChildModels() const
{
	return _stl->modelsVec.size();
}

