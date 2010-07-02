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
// Definitions
//========================================================================
MMat44	MMat44::IdentityMat(	1.0f, 0.0f, 0.0f, 0.0f,
								0.0f, 1.0f, 0.0f, 0.0f,
								0.0f, 0.0f, 1.0f, 0.0f,
								0.0f, 0.0f, 0.0f, 1.0f );


//===================
// MMat44::MMat44
//===================
MMat44::MMat44()
{
	Identity();
}


//===================
// MMat44::MMat44
//===================
MMat44::MMat44(	float _00, float _10, float _20, float _30,
				float _01, float _11, float _21, float _31,
				float _02, float _12, float _22, float _32,
				float _03, float _13, float _23, float _33 )
{
	// row 0
	_v[ 0] = _00;
	_v[ 1] = _10;
	_v[ 2] = _20;
	_v[ 3] = _30;

	// row 1
	_v[ 4] = _01;
	_v[ 5] = _11;
	_v[ 6] = _21;
	_v[ 7] = _31;

	// row 2
	_v[ 8] = _02;
	_v[ 9] = _12;
	_v[10] = _22;
	_v[11] = _32;

	// row 3
	_v[12] = _03;
	_v[13] = _13;
	_v[14] = _23;
	_v[15] = _33;
}


//===================
// MMat44::Identity
//===================
void			MMat44::Identity()
{
	Set(MMat44::IdentityMat._v);
}


//===================
// MMat44::TransformAffine
//===================
MVec3			MMat44::TransformNoPersp(const MVec3& v)
{
	float x = v.X();
	float y = v.Y();
	float z = v.Z();
	return MVec3(	x*Get(0, 0) + y*Get(0, 1) + z*Get(0, 2),
					x*Get(1, 0) + y*Get(1, 1) + z*Get(1, 2),
					x*Get(2, 0) + y*Get(2, 1) + z*Get(2, 2) );
}


//===================
// MMat44::TransformAffine
//===================
void			MMat44::TransformNoPersp(MVec3& v)
{
	float x = v.X();
	float y = v.Y();
	float z = v.Z();
	v.SetX(	x*Get(0, 0) + y*Get(0, 1) + z*Get(0, 2) );
	v.SetY(	x*Get(1, 0) + y*Get(1, 1) + z*Get(1, 2) );
	v.SetZ(	x*Get(2, 0) + y*Get(2, 1) + z*Get(2, 2) );
}


//===================
// MMat44::Set
//===================
void			MMat44::Set(const float* p)
{
	MemCpy( _v, p, sizeof(float)*4*4 );
}


//===================
// MMat44::GetTransform
//===================
MVec3			MMat44::GetTransform() const
{
	return MVec3( Get(0, 3), Get(1, 3), Get(2, 3) );
}


//===================
// MMat44::SetTransform
//===================
void			MMat44::SetTransform(const MVec3& t)
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
	Get(0, 0) = r.Get(0, 0);
	Get(0, 1) = r.Get(0, 1);
	Get(0, 2) = r.Get(0, 2);

	// Y axis
	Get(1, 0) = r.Get(1, 0);
	Get(1, 1) = r.Get(1, 1);
	Get(1, 2) = r.Get(1, 2);

	// Y axis
	Get(2, 0) = r.Get(2, 0);
	Get(2, 1) = r.Get(2, 1);
	Get(2, 2) = r.Get(2, 2);
}


//===================
// MMat44::operator(col, row)
//===================
float			MMat44::operator()(int col, int row) const
{
	return _v[4*row + col];
}


//===================
// MMat44::operator(col, row)
//===================
float&			MMat44::operator()(int col, int row)
{
	return _v[4*row + col];
}

