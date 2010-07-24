//========================================================================
//	file:		gr_types.cpp
//	author:		Shawn Presser 
//
// (c) 2010 Shawn Presser.  All Rights Reserved.
//========================================================================

//========================================================================
// Headers
//========================================================================
#include "e_common.h"
#include "gr_types.h"
#include "m_transform.h"
//========================================================================


//========================================================================
// Constants
//========================================================================
#define VEC2_SIZE				(sizeof(float) * 2)
#define VEC3_SIZE				(sizeof(float) * 3)
//========================================================================

//========================================================================
// Definitions
//========================================================================
SVec2	SVec2::Zero(0.0f, 0.0f);
SVec3	SVec3::Zero(0.0f, 0.0f, 0.0f);
//========================================================================

//========================================================================
// SVec2
//========================================================================

//===================
// SVec2::Set
//===================
void			SVec2::Set(float x, float y)
{
	SetX(x);
	SetY(y);
}


//===================
// SVec2::Set
//===================
void			SVec2::Set(const float* p)
{
	MemCpy(_v, p, VEC2_SIZE);
}


//===================
// SVec2::Set
//===================
void			SVec2::Set(const SVec2& v)
{
	MemCpy(_v, v._v, VEC2_SIZE);
}


//===================
// SVec2::Set
//===================
SVec2&			SVec2::operator =(const SVec2& v)
{
	MemCpy(_v, v._v, VEC2_SIZE);
	return *this;
}


//===================
// SVec2::Normalize
//===================
float			SVec2::Normalize()
{
	float mag(Mag());
	E_ASSERT(!FloatZero(mag));

	float invMag = 1.0f / mag;
	X() *= invMag;
	Y() *= invMag;

	return mag;
}


//===================
// SVec2::operator +
//===================
SVec2			SVec2::operator +(const SVec2& v) const
{
	return SVec2(
		_v[0] + v._v[0],
		_v[1] + v._v[1]);
}


//===================
// SVec2::operator +=
//===================
SVec2&			SVec2::operator +=(const SVec2& v)
{
	_v[0] += v._v[0];
	_v[1] += v._v[1];
	return *this;
}


//===================
// SVec2::operator -
//===================
SVec2			SVec2::operator -(const SVec2& v) const
{
	return SVec2(
		_v[0] - v._v[0],
		_v[1] - v._v[1]);
}


//===================
// SVec2::operator -=
//===================
SVec2&			SVec2::operator -=(const SVec2& v)
{
	_v[0] -= v._v[0];
	_v[1] -= v._v[1];
	return *this;
}


//===================
// SVec2::operator *
//===================
SVec2	operator *(float s, const SVec2& v)
{
	SVec2 r;
	r.X() = s * v.X();
	r.Y() = s * v.Y();
	return r;
}


//===================
// SVec2::operator *=
//===================
SVec2&			SVec2::operator *=(float s)
{
	_v[0] *= s;
	_v[1] *= s;
	return *this;
}
//========================================================================



//========================================================================
// SVec3
//========================================================================

//===================
// SVec3::Set
//===================
void			SVec3::Set(float x, float y, float z)
{
	SetX(x);
	SetY(y);
	SetZ(z);
}


//===================
// SVec3::Set
//===================
void			SVec3::Set(const float* p)
{
	MemCpy(_v, p, VEC3_SIZE);
}


//===================
// SVec3::Set
//===================
void			SVec3::Set(const SVec3& v)
{
	MemCpy(_v, v._v, VEC3_SIZE);
}


//===================
// SVec3::Set
//===================
SVec3&			SVec3::operator =(const SVec3& v)
{
	MemCpy(_v, v._v, VEC3_SIZE);
	return *this;
}


//===================
// SVec3::Normalize
//===================
float			SVec3::Normalize()
{
	float mag(Mag());
	E_ASSERT(!FloatZero(mag));

	float invMag = 1.0f / mag;
	X() *= invMag;
	Y() *= invMag;
	Z() *= invMag;

	return mag;
}


//===================
// SVec3::Cross
//===================
SVec3			SVec3::Cross(const SVec3& v) const
{
	// verify that neither vector has a zero magnitude.
	assert(!FloatZero(Mag()));
	assert(!FloatZero(v.Mag()));

	// | Ax Ay Az |
	// | Bx By Bz |
	SVec3 r;
	r.X() = Y()*v.Z() - Z()*v.Y();
	r.Y() = X()*v.Z() - Z()*v.X();
	r.Z() = X()*v.Y() - Y()*v.X();
	return r;
}


//===================
// SVec3::RotateTranslate
//===================
void			SVec3::RotateTranslate(const MTransform& m)
{
	MVec3 v(X(), Y(), Z());
	v = m.RotateTranslate(v);
	X() = v.X();
	Y() = v.Y();
	Z() = v.Z();
}


//===================
// SVec3::operator +
//===================
SVec3			SVec3::operator +(const SVec3& v) const
{
	return SVec3(
		_v[0] + v._v[0],
		_v[1] + v._v[1],
		_v[2] + v._v[2]);
}


//===================
// SVec3::operator +=
//===================
SVec3&			SVec3::operator +=(const SVec3& v)
{
	_v[0] += v._v[0];
	_v[1] += v._v[1];
	_v[2] += v._v[2];
	return *this;
}


//===================
// SVec3::operator -
//===================
SVec3			SVec3::operator -(const SVec3& v) const
{
	return SVec3(
		_v[0] - v._v[0],
		_v[1] - v._v[1],
		_v[2] - v._v[2]);
}


//===================
// SVec3::operator -=
//===================
SVec3&			SVec3::operator -=(const SVec3& v)
{
	_v[0] -= v._v[0];
	_v[1] -= v._v[1];
	_v[2] -= v._v[2];
	return *this;
}


//===================
// SVec3::operator *
//===================
SVec3	operator *(float s, const SVec3& v)
{
	SVec3 r;
	r.X() = s * v.X();
	r.Y() = s * v.Y();
	r.Z() = s * v.Z();
	return r;
}


//===================
// SVec3::operator *=
//===================
SVec3&			SVec3::operator *=(float s)
{
	_v[0] *= s;
	_v[1] *= s;
	_v[2] *= s;
	return *this;
}
//========================================================================

