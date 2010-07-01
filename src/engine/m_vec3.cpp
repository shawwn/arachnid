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


//===================
// MVec3::MVec3
//===================
MVec3::MVec3()
{
	// not initialized.
}


//===================
// MVec3::MVec3
//===================
MVec3::MVec3(float s)
{
	SetX(s);
	SetY(s);
	SetZ(s);
}


//===================
// MVec3::MVec3
//===================
MVec3::MVec3(float x, float y, float z)
{
	SetX(x);
	SetY(y);
	SetZ(z);
}


//===================
// MVec3::MVec3
//===================
MVec3::MVec3(const float* p)
{
	Set(p);
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
// MVec3::Dir
//===================
MVec3			MVec3::Dir(const MVec3& point)
{
	// compute the offset to the point.
	MVec3 r(point - *this);

	// normalize the offset to form a direction.
	float rMag(r.Mag());
	assert(rMag > 1e-5);
	float invMag(1.0f / rMag);
	r *= invMag;
	return r;
}


//===================
// MVec3::Cross
//===================
MVec3			MVec3::Cross(const MVec3& axis)
{
	MVec3 r;
	const MVec3& A(*this);
	const MVec3& B(axis);
	assert(A.Mag() > 1e-5);
	assert(B.Mag() > 1e-5);

	// | Ax Ay Az |
	// | Bx By Bz |
	r.SetX(A.Y()*B.Z() - A.Z()*B.Y());
	r.SetY(A.X()*B.Z() - A.Z()*B.X());
	r.SetZ(A.X()*B.Y() - A.Y()*B.X());
	return r;
}


//===================
// MVec3::operator +
//===================
MVec3			MVec3::operator +(const MVec3& v) const
{
	return MVec3(X() + v.X(), Y() + v.Y(), Z() + v.Z());
}


//===================
// MVec3::operator +=
//===================
MVec3&			MVec3::operator +=(const MVec3& v)
{
	SetX(X() + v.X());
	SetY(Y() + v.Y());
	SetZ(Z() + v.Z());
	return *this;
}


//===================
// MVec3::operator -
//===================
MVec3			MVec3::operator -(const MVec3& v) const
{
	return MVec3(X() - v.X(), Y() - v.Y(), Z() - v.Z());
}


//===================
// MVec3::operator -=
//===================
MVec3&			MVec3::operator -=(const MVec3& v)
{
	SetX(X() - v.X());
	SetY(Y() - v.Y());
	SetZ(Z() - v.Z());
	return *this;
}


//===================
// MVec3::operator *
//===================
MVec3	operator *(float s, const MVec3& v)
{
	return MVec3(s * v.X(), s * v.Y(), s * v.Z());
}


//===================
// MVec3::operator *=
//===================
MVec3&			MVec3::operator *=(float s)
{
	SetX(s * X());
	SetY(s * Y());
	SetZ(s * Z());
	return *this;
}

