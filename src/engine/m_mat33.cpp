//========================================================================
//	file:		m_mat33.cpp
//	author:		Shawn Presser 
//	date:		6/30/10
//
// (c) 2010 Shawn Presser.  All Rights Reserved.
//========================================================================

//========================================================================
// Headers
//========================================================================
#include "e_common.h"
#include "m_mat33.h"
#include "m_vec3.h"
//========================================================================

//========================================================================
// Constants
//========================================================================
#define MAT33_SIZE				(sizeof(float) * 3 * 3)
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
MMat33::MMat33(	float rotXx,	float rotYx,	float rotZx,
				float rotXy,	float rotYy,	float rotZy,
				float rotXz,	float rotYz,	float rotZz )
{
	// row 0
	_v[0] = rotXx;
	_v[1] = rotYx;
	_v[2] = rotZx;

	// row 1
	_v[3] = rotXy;
	_v[4] = rotYy;
	_v[5] = rotZy;

	// row 2
	_v[6] = rotXz;
	_v[7] = rotYz;
	_v[8] = rotZz;
}


//===================
// MMat33::XRot
//===================
MMat33			MMat33::XRot(float theta)
{
	// source:		http://en.wikipedia.org/wiki/Rotation_matrix#Basic_rotations

	float s = SIN(theta);
	float c = COS(theta);
	return MMat33(	1.0f,	0.0f,	 0.0f,
					0.0f,	c,		-s,
					0.0f,	s,		 c);
}


//===================
// MMat33::YRot
//===================
MMat33			MMat33::YRot(float theta)
{
	// source:		http://en.wikipedia.org/wiki/Rotation_matrix#Basic_rotations

	float s = SIN(theta);
	float c = COS(theta);
	return MMat33(	 c,		0.0f,	s,
					 0.0f,	1.0f,	0.0f,
					-s,		0.0f,	c);
}


//===================
// MMat33::ZRot
//===================
MMat33			MMat33::ZRot(float theta)
{
	// source:		http://en.wikipedia.org/wiki/Rotation_matrix#Basic_rotations

	float s = SIN(theta);
	float c = COS(theta);
	return MMat33(	c,		-s,		0.0f,
					s,		 c,		0.0f,
					0.0f,	 0.0f,	1.0f);
}


//===================
// MMat33::GetAxis
//===================
MVec3			MMat33::GetXAxis() const { return MVec3( Get(0, 0), Get(0, 1), Get(0, 2) ); }
MVec3			MMat33::GetYAxis() const { return MVec3( Get(1, 0), Get(1, 1), Get(1, 2) ); }
MVec3			MMat33::GetZAxis() const { return MVec3( Get(2, 0), Get(2, 1), Get(2, 2) ); }


//===================
// MMat33::Set
//===================
void			MMat33::Set(const float* p)
{
	MemCpy(_v, p, MAT33_SIZE);
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
// MMat33::SetAxis
//===================
void			MMat33::SetXAxis(const MVec3& v) { Get(0, 0) = v.X(); Get(0, 1) = v.Y(); Get(0, 2) = v.Z(); }
void			MMat33::SetYAxis(const MVec3& v) { Get(1, 0) = v.X(); Get(1, 1) = v.Y(); Get(1, 2) = v.Z(); }
void			MMat33::SetZAxis(const MVec3& v) { Get(2, 0) = v.X(); Get(2, 1) = v.Y(); Get(2, 2) = v.Z(); }


//===================
// MMat33::TransformAffine
//===================
MVec3			MMat33::Rotate(const MVec3& pt) const
{
	float x = pt.X();
	float y = pt.Y();
	float z = pt.Z();
	return MVec3(	x*Get(0, 0) + y*Get(0, 1) + z*Get(0, 2),
					x*Get(1, 0) + y*Get(1, 1) + z*Get(1, 2),
					x*Get(2, 0) + y*Get(2, 1) + z*Get(2, 2) );
}


//===================
// MMat33::TransformAffine
//===================
void			MMat33::Rotate(MVec3& pt) const
{
	float x = pt.X();
	float y = pt.Y();
	float z = pt.Z();
	pt.X() = x*Get(0, 0) + y*Get(0, 1) + z*Get(0, 2);
	pt.Y() = x*Get(1, 0) + y*Get(1, 1) + z*Get(1, 2);
	pt.Z() = x*Get(2, 0) + y*Get(2, 1) + z*Get(2, 2);
}


//===================
// MMat33::operator *
//===================
MMat33			MMat33::operator *(const MMat33& m) const
{
	MMat33 r(Zero);
	for (int i = 0; i < 3; ++i)
		for (int j = 0; j < 3; ++j)
			for (int k = 0; k < 3; ++k)
				r(i, j) += Get(i, k) * m(k, j);
	return r;
}


//===================
// MMat33::operator(col, row)
//===================
MMat33&			MMat33::operator *=(const MMat33& m)
{
	SetZero();
	for (int i = 0; i < 3; ++i)
		for (int j = 0; j < 3; ++j)
			for (int k = 0; k < 3; ++k)
				Get(i, j) += Get(i, k) * m(k, j);
	return *this;
}

