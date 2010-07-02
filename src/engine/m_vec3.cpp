//========================================================================
//	file:		m_vec3.cpp
//	author:		Shawn Presser 
//	date:		6/30/10
//
// (c) 2010 Shawn Presser.  All Rights Reserved.
//========================================================================

//========================================================================
// Headers
//========================================================================
#include "e_common.h"
#include "m_vec3.h"
//========================================================================

//========================================================================
// Constants
//========================================================================
#define VEC3_SIZE				(sizeof(float) * 3)
//========================================================================

//========================================================================
// Definitions
//========================================================================
MVec3	MVec3::Zero(0.0f, 0.0f, 0.0f);
//========================================================================


//===================
// MVec3::Set
//===================
void			MVec3::Set(float x, float y, float z)
{
	SetX(x);
	SetY(y);
	SetZ(z);
}


//===================
// MVec3::Set
//===================
void			MVec3::Set(const float* p)
{
	MemCpy(_v, p, VEC3_SIZE);
}


//===================
// MVec3::Set
//===================
void			MVec3::Set(const MVec3& v)
{
	MemCpy(_v, v._v, VEC3_SIZE);
}


//===================
// MVec3::Set
//===================
MVec3&			MVec3::operator =(const MVec3& v)
{
	MemCpy(_v, v._v, VEC3_SIZE);
	return *this;
}


//===================
// MVec3::MagSqr
//===================
float			MVec3::MagSqr() const
{
	return SQR(X()) + SQR(Y()) + SQR(Z());
}


//===================
// MVec3::Mag
//===================
float			MVec3::Mag() const
{
	return SQRT(MagSqr());
}


//===================
// MVec3::Dot
//===================
float			MVec3::Dot(const MVec3& v)
{
	return X()*v.X() + Y()*v.Y() + Z()*v.Z();
}


//===================
// MVec3::Normalize
//===================
float			MVec3::Normalize()
{
	// compute the current magnitude.
	float mag(Mag());

	// verify that the magnitude is not zero.
	assert(!FloatZero(mag));

	// divide by the magnitude.
	float invMag(1.0f / mag);
	Get(0) *= invMag;
	Get(1) *= invMag;
	Get(2) *= invMag;

	// return the previous magnitude.
	return mag;
}


//===================
// MVec3::Dir
//===================
MVec3			MVec3::Dir(const MVec3& point)
{
	// compute the offset to the point.
	MVec3 r(point - *this);

	// normalize the offset to form a direction.
	r.Normalize();

	// return the direction.
	return r;
}


//===================
// MVec3::Cross
//===================
MVec3			MVec3::Cross(const MVec3& v)
{
	// verify that neither vector has a zero magnitude.
	assert(!FloatSqrZero(MagSqr()));
	assert(!FloatSqrZero(v.MagSqr()));

	// | Ax Ay Az |
	// | Bx By Bz |
	MVec3 r;
	r.X() = Y()*v.Z() - Z()*v.Y();
	r.Y() = X()*v.Z() - Z()*v.X();
	r.Z() = X()*v.Y() - Y()*v.X();
	return r;
}


//===================
// MVec3::operator +
//===================
MVec3			MVec3::operator +(const MVec3& v) const
{
	MVec3 r;
	r.X() = X() + v.X();
	r.Y() = Y() + v.Y();
	r.Z() = Z() + v.Z();
	return r;
}


//===================
// MVec3::operator +=
//===================
MVec3&			MVec3::operator +=(const MVec3& v)
{
	X() += v.X();
	Y() += v.Y();
	Z() += v.Z();
	return *this;
}


//===================
// MVec3::operator -
//===================
MVec3			MVec3::operator -(const MVec3& v) const
{
	MVec3 r;
	r.X() = X() - v.X();
	r.Y() = Y() - v.Y();
	r.Z() = Z() - v.Z();
	return r;
}


//===================
// MVec3::operator -=
//===================
MVec3&			MVec3::operator -=(const MVec3& v)
{
	X() -= v.X();
	Y() -= v.Y();
	Z() -= v.Z();
	return *this;
}


//===================
// MVec3::operator *
//===================
MVec3	operator *(float s, const MVec3& v)
{
	MVec3 r;
	r.X() = s * v.X();
	r.Y() = s * v.Y();
	r.Z() = s * v.Z();
	return r;
}


//===================
// MVec3::operator *=
//===================
MVec3&			MVec3::operator *=(float s)
{
	X() *= s;
	Y() *= s;
	Z() *= s;
	return *this;
}

