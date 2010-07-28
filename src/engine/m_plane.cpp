//========================================================================
//	file:		m_plane.cpp
//	author:		Shawn Presser 
//
// (c) 2010 Shawn Presser.  All Rights Reserved.
//========================================================================

//========================================================================
// Headers
//========================================================================
#include "e_common.h"
#include "m_plane.h"
//========================================================================

//========================================================================
// Constants
//========================================================================
#define PLANE_SIZE				(sizeof(float) * 4)
//========================================================================

//========================================================================
// Definitions
//========================================================================
MPlane	MPlane::Zero(0.0f, 0.0f, 0.0f, 0.0f);
//========================================================================

//===================
// MPlane::FromPoints
//===================
MPlane			MPlane::FromPoints(const MVec3& A, const MVec3& B, const MVec3& C)
{
	MPlane r;
	r._norm = (B - A).Cross(C - A);
	r._norm.Normalize();
	r._d = r._norm.Dot(A);
	return r;
}


//===================
// MPlane::Set
//===================
void			MPlane::Set(const MVec3& norm, float d)
{
	_norm = norm;
	_d = d;
}


//===================
// MPlane::Set
//===================
void			MPlane::Set(const MPlane& p)
{
	_norm = p._norm;
	_d = p._d;
}


//===================
// MPlane::operator =
//===================
MPlane&			MPlane::operator =(const MPlane& p)
{
	_norm = p._norm;
	_d = p._d;
	return *this;
}


//===================
// MPlane::Normalize
//===================
float			MPlane::Normalize()
{
	return _norm.Normalize();
}


//===================
// MPlane::operator *
//===================
MPlane	operator *(float s, const MPlane& v)
{
	MPlane r(v);
	r._d *= s;
	return r;
}


//===================
// MPlane::operator *=
//===================
MPlane&			MPlane::operator *=(float s)
{
	_d *= s;
	return *this;
}
