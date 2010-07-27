//========================================================================
//	file:		gr_material.h
//	author:		Shawn Presser 
//	date:		7/6/10
//
// (c) 2010 Shawn Presser.  All Rights Reserved.
//========================================================================
#pragma once

//========================================================================
// Declarations
//========================================================================
class GrTexture;
//========================================================================

//========================================================================
// GrMaterial
//========================================================================
class ENGINE_API GrMaterial
{
private:
	GrTexture*			_textures[MTEX_COUNT];
	EMaterialType		_type;

public:
	GrMaterial();
	~GrMaterial();

	GrTexture*			GetTexture(EMaterialTexture which) const;
	void				SetTexture(EMaterialTexture which, GrTexture* val);

	EMaterialType		GetMaterialType() const				{ return _type; }
};
//========================================================================


