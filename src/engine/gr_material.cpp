//========================================================================
//	file:		gr_material.cpp
//	author:		Shawn Presser 
//	date:		7/6/10
//
// (c) 2010 Shawn Presser.  All Rights Reserved.
//========================================================================

//========================================================================
// Headers
//========================================================================
#include "e_common.h"
#include "gr_material.h"

// graphics headers.
#include "gr_texture.h"
//========================================================================


//===================
// GrMaterial::GrMaterial
//===================
GrMaterial::GrMaterial()
{
	for (uint i = 0; i < MT_COUNT; ++i)
		_textures[i] = NULL;
}


//===================
// GrMaterial::~GrMaterial
//===================
GrMaterial::~GrMaterial()
{
}

//===================
// GrMaterial::GetTexture
//===================
GrTexture*			GrMaterial::GetTexture(EMaterialTexture which) const
{
	E_VERIFY(which >= 0 && which < MT_COUNT, return NULL);
	return _textures[which];
}


//===================
// GrMaterial::SetTexture
//===================
void				GrMaterial::SetTexture(EMaterialTexture which, GrTexture* val)
{
	E_VERIFY(which >= 0 && which < MT_COUNT, return);
	_textures[which] = val;
}

