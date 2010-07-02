//========================================================================
//	file:		m_mat44.h
//	author:		Shawn Presser 
//	date:		6/30/10
//
// (c) 2010 Shawn Presser.  All Rights Reserved.
//========================================================================
#pragma once

//========================================================================
// Declarations
//========================================================================
class MVec3;
//========================================================================

//========================================================================
// MMat44
//========================================================================
class MMat44
{
private:
	float	_v[16];

public:
	MMat44();
	MMat44(	float _00, float _01, float _02, float _03,
			float _10, float _11, float _12, float _13,
			float _20, float _21, float _22, float _23,
			float _30, float _31, float _32, float _33 );
	MMat44(const float* p);

	static MMat44	IdentityMat;
	void			Identity();

	MVec3			TransformAffine(const MVec3& v);
	void			TransformAffine(MVec3& v);

	MMat44			operator *(const MMat44& m);
	MMat44&			operator *=(const MMat44& m);
};
//========================================================================
