//========================================================================
//	file:		gr_light.h
//	author:		Shawn Presser 
//	date:		7/27/10
//
// (c) 2010 Shawn Presser.  All Rights Reserved.
//========================================================================
#pragma once

//========================================================================
// Headers
//========================================================================
#include "m_vec3.h"
//========================================================================

//========================================================================
// Declarations
//========================================================================
class MTransform;
typedef float		(*LightFalloffFunc)(float t);
//========================================================================

//========================================================================
// Constants
//========================================================================
#define LIGHT_FALLOFF_SAMPLES		128
//========================================================================

//========================================================================
// GrLight
//========================================================================
class ENGINE_API GrLight
{
private:
	SVec4				_col;

	MVec3				_pos;
	float				_radius;

	float				_falloff[LIGHT_FALLOFF_SAMPLES];

public:
	GrLight();
	~GrLight();

	void				Transform(GrLight& result, const MTransform& xform) const;

	const SVec4&		GetColor() const			{ return _col; }
	void				SetColor(const SVec4& col)	{ _col = col; }

	const MVec3&		GetPos() const				{ return _pos; }
	void				SetPos(const MVec3& pos)	{ _pos = pos; }

	float				GetRadius() const			{ return _radius; }
	void				SetRadius(float rad)		{ _radius = MAX(0.0f, rad); }

	const float*		GetFalloff() const			{ return _falloff; }
	void				SetFalloff(LightFalloffFunc fn);

	static float		LinearFalloff(float t)		{ return 1.0f - t; }
	static float		RoundedFalloff(float t)		{ return 1.0f - t*t*t; }

};
//========================================================================


