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
// Definitions
//========================================================================
MMat33	MMat33::IdentityMat(	1.0f, 0.0f, 0.0f,
								0.0f, 1.0f, 0.0f,
								0.0f, 0.0f, 1.0f );


//===================
// MMat33::MMat33
//===================
MMat33::MMat33()
{
	Identity();
}


//===================
// MMat33::MMat33
//===================
MMat33::MMat33(	float _00, float _10, float _20,
				float _01, float _11, float _21,
				float _02, float _12, float _22 )
{
	// row 0
	_v[0] = _00;
	_v[1] = _10;
	_v[2] = _20;

	// row 1
	_v[3] = _01;
	_v[4] = _11;
	_v[5] = _21;

	// row 2
	_v[6] = _02;
	_v[7] = _12;
	_v[8] = _22;
}


//===================
// MMat33::Identity
//===================
void			MMat33::Identity()
{
	MemCpy(_v, MMat33::IdentityMat._v, sizeof(float) * 9);
}


//===================
// MMat33::RotX
//===================
MMat33			MMat33::RotX(float theta)
{
	// source:		http://en.wikipedia.org/wiki/Rotation_matrix#Basic_rotations

	float s = SIN(theta);
	float c = COS(theta);
	return MMat33(	1.0f,	0.0f,	 0.0f,
					0.0f,	c,		-s,
					0.0f,	s,		 c);
}


//===================
// MMat33::RotY
//===================
MMat33			MMat33::RotY(float theta)
{
	// source:		http://en.wikipedia.org/wiki/Rotation_matrix#Basic_rotations

	float s = SIN(theta);
	float c = COS(theta);
	return MMat33(	 c,		0.0f,	s,
					 0.0f,	1.0f,	0.0f,
					-s,		0.0f,	c);
}


//===================
// MMat33::RotZ
//===================
MMat33			MMat33::RotZ(float theta)
{
	// source:		http://en.wikipedia.org/wiki/Rotation_matrix#Basic_rotations

	float s = SIN(theta);
	float c = COS(theta);
	return MMat33(	c,		-s,		0.0f,
					s,		 c,		0.0f,
					0.0f,	 0.0f,	1.0f);
}


//===================
// MMat33::TransformAffine
//===================
MVec3			MMat33::Rotate(const MVec3& v)
{
	float x = v.X();
	float y = v.Y();
	float z = v.Z();
	return MVec3(	x*Get(0, 0) + y*Get(0, 1) + z*Get(0, 2),
					x*Get(1, 0) + y*Get(1, 1) + z*Get(1, 2),
					x*Get(2, 0) + y*Get(2, 1) + z*Get(2, 2) );
}


//===================
// MMat33::TransformAffine
//===================
void			MMat33::Rotate(MVec3& v)
{
	float x = v.X();
	float y = v.Y();
	float z = v.Z();
	v.SetX(	x*Get(0, 0) + y*Get(0, 1) + z*Get(0, 2) );
	v.SetY(	x*Get(1, 0) + y*Get(1, 1) + z*Get(1, 2) );
	v.SetZ(	x*Get(2, 0) + y*Get(2, 1) + z*Get(2, 2) );
}


//===================
// MMat33::Set
//===================
void			MMat33::Set(const float* p)
{
	MemCpy(_v, p, sizeof(float)*3*3 );
}


//===================
// MMat33::GetAxis
//===================
MVec3			MMat33::GetXAxis() const { return MVec3( Get(0, 0), Get(0, 1), Get(0, 2) ); }
MVec3			MMat33::GetYAxis() const { return MVec3( Get(1, 0), Get(1, 1), Get(1, 2) ); }
MVec3			MMat33::GetZAxis() const { return MVec3( Get(2, 0), Get(2, 1), Get(2, 2) ); }


//===================
// MMat33::SetAxis
//===================
void			MMat33::SetXAxis(const MVec3& v) { Get(0, 0) = v.X(); Get(0, 1) = v.Y(); Get(0, 2) = v.Z(); }
void			MMat33::SetYAxis(const MVec3& v) { Get(1, 0) = v.X(); Get(1, 1) = v.Y(); Get(1, 2) = v.Z(); }
void			MMat33::SetZAxis(const MVec3& v) { Get(2, 0) = v.X(); Get(2, 1) = v.Y(); Get(2, 2) = v.Z(); }


//===================
// MMat33::operator(col, row)
//===================
float			MMat33::operator()(int col, int row) const
{
	return _v[3*row + col];
}


//===================
// MMat33::operator(col, row)
//===================
float&			MMat33::operator()(int col, int row)
{
	return _v[3*row + col];
}

