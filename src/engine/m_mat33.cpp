//========================================================================
//	file:		m_mat33.cpp
//	author:		Shawn Presser 
//
// (c) 2010 Shawn Presser.  All Rights Reserved.
//========================================================================

//========================================================================
// Headers
//========================================================================
#include "e_common.h"
#include "m_mat33.h"
#include "m_vec3.h"
#include "m_quat.h"
//========================================================================

//========================================================================
// Constants
//========================================================================
#define MAT33_SIZE				(sizeof(float) * 3 * 3)
#define UNINITIALIZED_MAT33		true
//========================================================================

//========================================================================
// Definitions
//========================================================================
MMat33	MMat33::Identity(	1.0f, 0.0f, 0.0f,
							0.0f, 1.0f, 0.0f,
							0.0f, 0.0f, 1.0f );

MMat33	MMat33::Zero(		0.0f, 0.0f, 0.0f,
							0.0f, 0.0f, 0.0f,
							0.0f, 0.0f, 0.0f );


//===================
// MMat33::MMat33
//===================
MMat33::MMat33(	float _00,	float _01,	float _02,
				float _10,	float _11,	float _12,
				float _20,	float _21,	float _22 )
{
	// X basis
	Get(0, 0) = _00;
	Get(0, 1) = _01;
	Get(0, 2) = _02;

	// Y basis
	Get(1, 0) = _10;
	Get(1, 1) = _11;
	Get(1, 2) = _12;

	// Z basis
	Get(2, 0) = _20;
	Get(2, 1) = _21;
	Get(2, 2) = _22;
}


//===================
// MMat33::XRot
//===================
MMat33			MMat33::FromXRot(float theta)
{
	// source:		http://en.wikipedia.org/wiki/Rotation_matrix#Basic_rotations

	float s = SIN(theta);
	float c = COS(theta);
	return MMat33(	1.0f,	0.0f,	 0.0f,
					0.0f,	c,		 -s,
					0.0f,	s,		 c);
}


//===================
// MMat33::YRot
//===================
MMat33			MMat33::FromYRot(float theta)
{
	// source:		http://en.wikipedia.org/wiki/Rotation_matrix#Basic_rotations

	float s = SIN(theta);
	float c = COS(theta);
	return MMat33(	c,		0.0f,	s,
					0.0f,	1.0f,	0.0f,
					-s,		0.0f,	c);
}


//===================
// MMat33::ZRot
//===================
MMat33			MMat33::FromZRot(float theta)
{
	// source:		http://en.wikipedia.org/wiki/Rotation_matrix#Basic_rotations

	float s = SIN(theta);
	float c = COS(theta);
	return MMat33(	c,		-s,		0.0f,
					s,		c,		0.0f,
					0.0f,	0.0f,	1.0f);
}


//===================
// MMat33::FromAxisAngle
//===================
MMat33			MMat33::FromAxisAngle(const MVec3& axis, float angle)
{
	MMat33 r(UNINITIALIZED_MAT33);

	float c = COS(angle);
	float s = SIN(angle);
	float t = 1.0f - c;

	// normalize the axis.
	float mag = SQRT(axis.Mag());
	E_VERIFY(!FloatZero(mag), mag = 1.0f);
	float invMag = 1.0f / mag;
	float x = axis.X() * invMag;
	float y = axis.Y() * invMag;
	float z = axis.Z() * invMag;

	r(0, 0) = c + x*x*t;
	r(1, 1) = c + y*y*t;
	r(2, 2) = c + z*z*t;

	float tmp1, tmp2;

	tmp1 = x*y*t;
	tmp2 = z*s;
	r(1, 0) = tmp1 + tmp2;
	r(0, 1) = tmp1 - tmp2;

	tmp1 = x*z*t;
	tmp2 = y*s;
	r(2, 0) = tmp1 - tmp2;
	r(0, 2) = tmp1 + tmp2;

	tmp1 = y*z*t;
	tmp2 = x*s;
	r(2, 1) = tmp1 + tmp2;
	r(1, 2) = tmp1 - tmp2;

	return r;
}


//===================
// MMat33::ToAxisAngle
//===================
void			MMat33::ToAxisAngle(MVec3& axis, float& angle) const
{
	float x, y, z;

	// margin to allow for rounding errors
	float epsilon = 0.01f;

	// margin to distinguish between 0 and 180 degrees
	float epsilon2 = 0.1f;

	// verify that we are orthogonal.
	E_ASSERT(IsOrtho());

	if (	(ABS(Get(0, 1) - Get(1, 0))< epsilon)
		&&	(ABS(Get(0, 2) - Get(2, 0))< epsilon)
		&&	(ABS(Get(1, 2) - Get(2, 1))< epsilon))
	{
			// singularity found
			// first check for identity matrix which must have  + 1 for all terms
			// in leading diagonal and zero in other terms
			if (	(ABS(Get(0, 1) + Get(1, 0)) < epsilon2)
				&&	(ABS(Get(0, 2) + Get(2, 0)) < epsilon2)
				&&	(ABS(Get(1, 2) + Get(2, 1)) < epsilon2)
				&&	(ABS(Get(0, 0) + Get(1, 1) + Get(2, 2) - 3.0f) < epsilon2))
			{
				// this singularity is identity matrix so zero angle, arbitrary axis
				axis.Set(0.0f, 1.0f, 0.0f);
				angle = 0.0f;
				return;
			}

			// otherwise this singularity is angle = 180
			angle = M_PI;
			float xx = (Get(0, 0) + 1) / 2.0f;
			float yy = (Get(1, 1) + 1) / 2.0f;
			float zz = (Get(2, 2) + 1) / 2.0f;
			float xy = (Get(0, 1) + Get(1, 0)) / 4.0f;
			float xz = (Get(0, 2) + Get(2, 0)) / 4.0f;
			float yz = (Get(1, 2) + Get(2, 1)) / 4.0f;
			if ((xx > yy) && (xx > zz))
			{
				// Get(0, 0) is the largest diagonal term
				if (xx < epsilon)
				{
					x = 0.0f;
					y = 0.7071f;
					z = 0.7071f;
				}
				else
				{
					x = SQRT(xx);
					y = xy / x;
					z = xz / x;
				}
			}
			else if (yy > zz)
			{
				// Get(1, 1) is the largest diagonal term
				if (yy< epsilon)
				{
					x = 0.7071f;
					y = 0.0f;
					z = 0.7071f;
				}
				else
				{
					y = SQRT(yy);
					x = xy / y;
					z = yz / y;
				}	
			}
			else
			{
				// Get(2, 2) is the largest diagonal term so base result on this
				if (zz< epsilon)
				{
					x = 0.7071f;
					y = 0.7071f;
					z = 0.0f;
				}
				else
				{
					z = SQRT(zz);
					x = xz / z;
					y = yz / z;
				}
			}
			// return 180 deg rotation
			axis.Set(x, y, z);
			return;
	}

	// as we have reached here there are no singularities so we can handle normally
	float s = SQRT(
			(Get(2, 1) - Get(1, 2))*(Get(2, 1) - Get(1, 2))
		 +	(Get(0, 2) - Get(2, 0))*(Get(0, 2) - Get(2, 0))
		 +	(Get(1, 0) - Get(0, 1))*(Get(1, 0) - Get(0, 1))); // used to normalize

	E_VERIFY(ABS(s) >= 0.001f, s = 1.0f);
	// prevent divide by zero, should not happen if matrix is orthogonal and should be
	// caught by singularity test above, but I've left it in just in case
	s = 1.0f / s;

	angle = ACOS((Get(0, 0) + Get(1, 1) + Get(2, 2) - 1.0f) / 2.0f);
	x = (Get(2, 1) - Get(1, 2)) * s;
	y = (Get(0, 2) - Get(2, 0)) * s;
	z = (Get(1, 0) - Get(0, 1)) * s;
	axis.Set(x, y, z);
}


//===================
// MMat33::Set
//===================
void			MMat33::Set(const MMat33& m)
{
	MemCpy(_v, m._v, MAT33_SIZE);
}


//===================
// MMat33::operator =
//===================
MMat33&			MMat33::operator =(const MMat33& m)
{
	MemCpy(_v, m._v, MAT33_SIZE);
	return *this;
}


//===================
// MMat33::IsOrtho
//===================
bool			MMat33::IsOrtho() const
{
	// verify that there is no shearing.
	MVec3 xBasis(Get(0, 0), Get(0, 1), Get(0, 2));
	MVec3 yBasis(Get(1, 0), Get(1, 1), Get(1, 2));
	MVec3 zBasis(Get(2, 0), Get(2, 1), Get(2, 2));
	float xy(xBasis.Dot(yBasis));
	float yz(yBasis.Dot(zBasis));
	float zx(zBasis.Dot(xBasis));
	if (!FloatZero(xy, 0.001f))
		return false;
	if (!FloatZero(yz, 0.001f))
		return false;
	if (!FloatZero(zx, 0.001f))
		return false;

	return true;
}


//===================
// MMat33::RotatePoint
//===================
MVec3			MMat33::RotatePoint(const MVec3& pt) const
{
	float x = pt.X();
	float y = pt.Y();
	float z = pt.Z();

	//	[ Pout.x ]   [ Xx Xy Xz ][ Px ]   [ Px*Xx + Py*Xy + Pz*Xz ]
	//	[ Pout.y ] = [ Yx Yy Yz ][ Py ] = [ Px*Yx + Py*Yy + Pz*Yz ]
	//	[ Pout.z ]   [ Zx Zy Zz ][ Pz ]   [ Px*Zx + Py*Zy + Pz*Zz ]
	//
	// or
	//
	//	for (int i = 0; i < 3; ++i)
	//		for (int j = 0; j < 1; ++j)
	//			for (int k = 0; k < 3; ++k)
	//				r(i, j) += Get(i, k) * p(k, j);
	//
	MVec3 r(
		DOT3(x, y, z, Get(0, 0), Get(0, 1), Get(0, 2)),
		DOT3(x, y, z, Get(1, 0), Get(1, 1), Get(1, 2)),
		DOT3(x, y, z, Get(2, 0), Get(2, 1), Get(2, 2)));

	return r;
}


//===================
// MMat33::RotatePointXYZ
//===================
void			MMat33::RotatePointXYZ(float& Px, float& Py, float& Pz) const
{
	float x = Px;
	float y = Py;
	float z = Pz;

	//	[ Pout.x ]   [ Xx Xy Xz ][ Px ]   [ Px*Xx + Py*Xy + Pz*Xz ]
	//	[ Pout.y ] = [ Yx Yy Yz ][ Py ] = [ Px*Yx + Py*Yy + Pz*Yz ]
	//	[ Pout.z ]   [ Zx Zy Zz ][ Pz ]   [ Px*Zx + Py*Zy + Pz*Zz ]
	//
	// or
	//
	//	for (int i = 0; i < 3; ++i)
	//		for (int j = 0; j < 1; ++j)
	//			for (int k = 0; k < 3; ++k)
	//				r(i, j) += Get(i, k) * p(k, j);
	//
	Px = DOT3(x, y, z, Get(0, 0), Get(0, 1), Get(0, 2));
	Py = DOT3(x, y, z, Get(1, 0), Get(1, 1), Get(1, 2));
	Pz = DOT3(x, y, z, Get(2, 0), Get(2, 1), Get(2, 2));
}


//===================
// MMat33::CalcTranspose
//===================
MMat33			MMat33::CalcTranspose() const
{
	MMat33 r(UNINITIALIZED_MAT33);
	for (int row = 0; row < 3; ++row)
		for (int col = 0; col < 3; ++col)
			r(row, col) = Get(col, row);
	return r;
}


//===================
// MMat33::CalcInverseOrtho
//===================
MMat33			MMat33::CalcInverseOrtho() const
{
	// this matrix represents a rotation without any shearing (orthogonal).
	// therefore, our opposite (inverse) is our transpose.
	E_ASSERT(IsOrtho());
	return CalcTranspose();
}


//===================
// MMat33::operator *
//===================
MMat33			MMat33::operator *(const MMat33& m) const
{
	// the incoming matrix is applied first, followed by us (postmultiply).
	const MMat33& appliedFirst(m);
	const MMat33& appliedSecond(*this);

	MMat33 r(Zero);
	for (int i = 0; i < 3; ++i)
		for (int j = 0; j < 3; ++j)
			for (int k = 0; k < 3; ++k)
				r(i, j) += appliedSecond(i, k) * appliedFirst(k, j);
	return r;
}


//===================
// MMat33::Compare
//===================
bool			MMat33::Compare(const MMat33& m, float epsilon) const
{
	return FloatsEqual(_v, m._v, 3 * 3, epsilon);
}

