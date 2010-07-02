//========================================================================
//	file:		gr_scene.cpp
//	author:		Shawn Presser 
//	date:		7/2/10
//
// (c) 2010 Shawn Presser.  All Rights Reserved.
//========================================================================

//========================================================================
// Headers
//========================================================================
#include "e_common.h"
#include "gr_scene.h"
#include "gr_model.h"
//========================================================================


//===================
// GrScene::GrScene
//===================
GrScene::GrScene(const wstring& name)
: _name(name)
, _model(GrModel::Create(_T("scene"), name))
{
}


//===================
// GrScene::~GrScene
//===================
GrScene::~GrScene()
{
	E_DELETE("scene", _model);
}

