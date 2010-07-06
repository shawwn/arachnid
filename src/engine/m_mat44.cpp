//========================================================================
//	file:		m_mat44.cpp
//	author:		Shawn Presser 
//	date:		6/30/10
//
// (c) 2010 Shawn Presser.  All Rights Reserved.
//========================================================================

//========================================================================
// Headers
//========================================================================
#include "e_common.h"
#include "m_mat44.h"
#include "m_mat33.h"
#include "m_vec3.h"
//========================================================================

//========================================================================
// Constants
//========================================================================
#define MAT44_SIZE				(sizeof(float) * 4 * 4)
//========================================================================

//========================================================================
// Definitions
//========================================================================
MMat44	MMat44::Identity(	1.0f, 0.0f, 0.0f, 0.0f,
							0.0f, 1.0f, 0.0f, 0.0f,
							0.0f, 0.0f, 1.0f, 0.0f,
							0.0f, 0.0f, 0.0f, 1.0f );

MMat44	MMat44::Zero(		0.0f, 0.0f, 0.0f, 0.0f,
							0.0f, 0.0f, 0.0f, 0.0f,
							0.0f, 0.0f, 0.0f, 0.0f,
							0.0f, 0.0f, 0.0f, 0.0f );
//========================================================================


//===================
// MMat44::MMat44
//===================
MMat44::MMat44(	float rotXx,	float rotYx,	float rotZx,	float Wx,
				float rotXy,	float rotYy,	float rotZy,	float Wy,
				float rotXz,	float rotYz,	float rotZz,	float Wz,
				float Tx,		float Ty,		float Tz,		float Ww )
{
	// row 0
	_v[ 0] = rotXx;
	_v[ 1] = rotYx;
	_v[ 2] = rotZx;
	_v[ 3] = Wx;

	// row 1
	_v[ 4] = rotXy;
	_v[ 5] = rotYy;
	_v[ 6] = rotZy;
	_v[ 7] = Wy;

	// row 2
	_v[ 8] = rotXz;
	_v[ 9] = rotYz;
	_v[10] = rotZz;
	_v[11] = Wz;

	// row 3
	_v[12] = Tx;
	_v[13] = Ty;
	_v[14] = Tz;
	_v[15] = Ww;

	// regardless of whether we're using DirectX or OpenGL,
	// the translation component (Tx, Ty, Tz) of the matrix
	// is always stored at memory locations [12] [13] [14].
}


//===================
// MMat44::Translation
//===================
MMat44			MMat44::Translation(const MVec3& delta)
{
	MMat44 result;
	result.SetTranslation(delta);
	return result;
}


//===================
// MMat44::Set
//===================
void			MMat44::Set(const float* p)
{
	MemCpy(_v, p, MAT44_SIZE);
}


//===================
// MMat44::Set
//===================
void			MMat44::Set(const MMat44& m)
{
	MemCpy(_v, m._v, MAT44_SIZE);
}


//===================
// MMat44::operator =
//===================
MMat44&			MMat44::operator =(const MMat44& m)
{
	MemCpy(_v, m._v, MAT44_SIZE);
	return *this;
}


//===================
// MMat44::GetTranslation
//===================
MVec3			MMat44::GetTranslation() const
{
	return MVec3( Get(0, 3), Get(1, 3), Get(2, 3) );
}


//===================
// MMat44::SetTranslation
//===================
void			MMat44::SetTranslation(const MVec3& t)
{
	Get(0, 3) = t.X();
	Get(1, 3) = t.Y();
	Get(2, 3) = t.Z();
}


//===================
// MMat44::GetRot
//===================
MMat33			MMat44::GetRot() const
{
	return MMat33( 
		Get(0, 0), Get(1, 0), Get(2, 0),
		Get(0, 1), Get(1, 1), Get(2, 1),
		Get(0, 2), Get(1, 2), Get(2, 2) );
}


//===================
// MMat44::SetRot
//===================
void			MMat44::SetRot(const MMat33& r)
{
	// X axis
	Get(0, 0) = r(0, 0);
	Get(0, 1) = r(0, 1);
	Get(0, 2) = r(0, 2);

	// Y axis
	Get(1, 0) = r(1, 0);
	Get(1, 1) = r(1, 1);
	Get(1, 2) = r(1, 2);

	// Y axis
	Get(2, 0) = r(2, 0);
	Get(2, 1) = r(2, 1);
	Get(2, 2) = r(2, 2);
}


//===================
// MMat44::TransformAffine
//===================
MVec3			MMat44::RotateTranslate(const MVec3& pt)
{
	MVec3 r;
	float x = pt.X();
	float y = pt.Y();
	float z = pt.Z();
	r.X() = x*Get(0, 0) + y*Get(0, 1) + z*Get(0, 2) + Get(0, 3);
	r.Y() = x*Get(1, 0) + y*Get(1, 1) + z*Get(1, 2) + Get(1, 3);
	r.Z() = x*Get(2, 0) + y*Get(2, 1) + z*Get(2, 2) + Get(2, 3);
	return r;
}


//===================
// MMat44::TransformAffine
//===================
void			MMat44::RotateTranslate(MVec3& pt)
{
	float x = pt.X();
	float y = pt.Y();
	float z = pt.Z();
	pt.X() = x*Get(0, 0) + y*Get(0, 1) + z*Get(0, 2) + Get(0, 3);
	pt.Y() = x*Get(1, 0) + y*Get(1, 1) + z*Get(1, 2) + Get(1, 3);
	pt.Z() = x*Get(2, 0) + y*Get(2, 1) + z*Get(2, 2) + Get(2, 3);
}


//===================
// MMat44::operator *
//===================
MMat44			MMat44::operator *(const MMat44& m) const
{
	MMat44 r(Zero);
	for (int i = 0; i < 4; ++i)
		for (int j = 0; j < 4; ++j)
			for (int k = 0; k < 4; ++k)
				r(i, j) += Get(i, k) * m(k, j);
	return r;
}


//===================
// MMat44::operator(col, row)
//===================
MMat44&			MMat44::operator *=(const MMat44& m)
{
	SetZero();
	for (int i = 0; i < 4; ++i)
		for (int j = 0; j < 4; ++j)
			for (int k = 0; k < 4; ++k)
				Get(i, j) += Get(i, k) * m(k, j);
	return *this;
}

