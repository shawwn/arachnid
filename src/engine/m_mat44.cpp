//========================================================================
//	file:		m_mat44.cpp
//	author:		Shawn Presser 
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
MMat44::MMat44(	float _00,	float _01,	float _02,	float _03,
				float _10,	float _11,	float _12,	float _13,
				float _20,	float _21,	float _22,	float _23,
				float _30,	float _31,	float _32,	float _33)
{
	// X basis
	Get(0, 0) = _00;
	Get(0, 1) = _01;
	Get(0, 2) = _02;
	Get(0, 3) = _03;

	// Y basis
	Get(1, 0) = _10;
	Get(1, 1) = _11;
	Get(1, 2) = _12;
	Get(1, 3) = _13;

	// Z basis
	Get(2, 0) = _20;
	Get(2, 1) = _21;
	Get(2, 2) = _22;
	Get(2, 3) = _23;

	// projection basis
	Get(3, 0) = _30;
	Get(3, 1) = _31;
	Get(3, 2) = _32;
	Get(3, 3) = _33;
}


//===================
// MMat44::MMat44(rot, trans)
//===================
MMat44::MMat44(const MMat33& rot, const MVec3& trans)
{
	SetIdentity();
	SetRot(rot);
	SetTrans(trans);
}


//===================
// MMat44::MMat44(rot)
//===================
MMat44::MMat44(const MMat33& rot)
{
	SetIdentity();
	SetRot(rot);
}


//===================
// MMat44::MMat44(trans)
//===================
MMat44::MMat44(const MVec3& trans)
{
	SetIdentity();
	SetTrans(trans);
}


//===================
// MMat44::Set
//===================
void			MMat44::Set(const MMat44& m)
{
	MemCpy(_v, m._v, MAT44_SIZE);
}


//===================
// MMat44::IsOrtho
//===================
bool			MMat44::IsOrtho() const
{
	// TODO: Clean this up.
	
	// verify that there is no projection.
	if (!FloatZero(Get(3, 0)))
		return false;
	if (!FloatZero(Get(3, 1)))
		return false;
	if (!FloatZero(Get(3, 2)))
		return false;
	if (!FloatEqual(Get(3, 3), 1.0f))
		return false;

	return GetRot().IsOrtho();
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
// MMat44::GetRot
//===================
MMat33			MMat44::GetRot() const
{
	return MMat33( 
		Get(0, 0), Get(0, 1), Get(0, 2),
		Get(1, 0), Get(1, 1), Get(1, 2),
		Get(2, 0), Get(2, 1), Get(2, 2) );
}


//===================
// MMat44::SetRot
//===================
void			MMat44::SetRot(const MMat33& m)
{
	// X axis
	Get(0, 0) = m(0, 0);
	Get(0, 1) = m(0, 1);
	Get(0, 2) = m(0, 2);

	// Y axis
	Get(1, 0) = m(1, 0);
	Get(1, 1) = m(1, 1);
	Get(1, 2) = m(1, 2);

	// Y axis
	Get(2, 0) = m(2, 0);
	Get(2, 1) = m(2, 1);
	Get(2, 2) = m(2, 2);
}


//===================
// MMat44::GetTrans
//===================
MVec3			MMat44::GetTrans() const
{
	return MVec3(Get(0, 3), Get(1, 3), Get(2, 3));
}


//===================
// MMat44::SetTrans
//===================
void			MMat44::SetTrans(const MVec3& t)
{
	Get(0, 3) = t.X();
	Get(1, 3) = t.Y();
	Get(2, 3) = t.Z();
}


//===================
// MMat44::CalcTranspose
//===================
MMat44			MMat44::CalcTranspose()
{
	MMat44 r(Zero);
	for (int row = 0; row < 4; ++row)
		for (int col = 0; col < 4; ++col)
			r(row, col) = Get(col, row);
	return r;
}


//===================
// MMat44::operator *
//===================
MMat44			MMat44::operator *(const MMat44& m) const
{
	// the incoming matrix is applied first, followed by us (postmultiply).
	const MMat44& appliedFirst(m);
	const MMat44& appliedSecond(*this);

	MMat44 r(Zero);
	for (int i = 0; i < 4; ++i)
		for (int j = 0; j < 4; ++j)
			for (int k = 0; k < 4; ++k)
				r(i, j) += appliedSecond(i, k) * appliedFirst(k, j);
	return r;
}


//===================
// MMat44::Compare
//===================
bool			MMat44::Compare(const MMat44& m, float epsilon) const
{
	return FloatsEqual(_v, m._v, 4 * 4, epsilon);
}