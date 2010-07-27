//========================================================================
//	file:		gr_material.cpp
//	author:		Shawn Presser 
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
: _type(MAT_SOLID)
{
	for (uint i = 0; i < MTEX_COUNT; ++i)
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
	E_VERIFY(which >= 0 && which < MTEX_COUNT, return NULL);
	return _textures[which];
}


//===================
// GrMaterial::SetTexture
//===================
void				GrMaterial::SetTexture(EMaterialTexture which, GrTexture* val)
{
	E_VERIFY(which >= 0 && which < MTEX_COUNT, return);
	_textures[which] = val;
}

