//========================================================================
//	file:		gr_light.cpp
//	author:		Shawn Presser 
//
// (c) 2010 Shawn Presser.  All Rights Reserved.
//========================================================================

//========================================================================
// Headers
//========================================================================
#include "e_common.h"
#include "gr_light.h"

// math headers.
#include "m_transform.h"
//========================================================================


//===================
// GrLight::GrLight
//===================
GrLight::GrLight()
: _pos(MVec3(0.0f, 0.0f, 0.0f))
, _radius(0.0f)
, _col(SVec4(1.0f, 1.0f, 1.0f, 1.0f))
{
	SetFalloff(LinearFalloff);
}


//===================
// GrLight::~GrLight
//===================
GrLight::~GrLight()
{
}


//===================
// GrLight::Transform
//===================
void				GrLight::Transform(GrLight& result, const MTransform& xform) const
{
	result = *this;
	result._pos = xform.RotateTranslate(result._pos);
}


//===================
// GrLight::Transform
//===================
void				GrLight::SetFalloff(LightFalloffFunc fn)
{
	for (uint i = 0; i < LIGHT_FALLOFF_SAMPLES; ++i)
	{
		float t = i / (float)LIGHT_FALLOFF_SAMPLES;
		_falloff[i] = fn(t);
	}
}