//========================================================================
//	file:		m_plane.h
//	author:		Shawn Presser 
//	date:		7/28/10
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
// MPlane
//========================================================================
class ENGINE_API MPlane
{
private:
	MVec3			_norm;
	float			_d;

public:
	// ctors.
	MPlane()									{ Set(Zero); }
	MPlane(const MPlane& v)						{ Set(v); }
	MPlane(float a, float b, float c, float d)	{ Set(MVec3(a, b, c), d); }

	static MPlane	FromPoints(const MVec3& A, const MVec3& B, const MVec3& C);

	// identity / zero.
	static MPlane	Zero;
	void			SetZero()					{ Set(Zero); }

	// accessors.
	const MVec3&	Norm() const				{ return _norm; }
	MVec3&			Norm()						{ return _norm; }
	float			D() const					{ return _d; }
	float&			D()							{ return _d; }

	// setters.
	void			Set(const MVec3& norm, float d);
	void			Set(const MPlane& p);
	MPlane&			operator =(const MPlane& p);

	// normalizes this plane and returns the previous magnitude.
	float			Normalize();

	friend ENGINE_API MPlane	operator *(float s, const MPlane& p);
	MPlane&						operator *=(float s);
};
//========================================================================


