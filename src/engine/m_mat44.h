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
class MMat33;
//========================================================================

//========================================================================
// MMat44
//		Row-major 4x4 matrix.
//========================================================================
class ENGINE_API MMat44
{
private:
	float	_v[16];
public:
	// ctors.
	MMat44()												{ SetIdentity(); }
	MMat44(const MMat44& m)									{ Set(m); }
	MMat44(	float _00,	float _01,	float _02,	float _03,
			float _10,	float _11,	float _12,	float _13,
			float _20,	float _21,	float _22,	float _23,
			float _30,	float _31,	float _32,	float _33);

	// constructs a matrix that represents a 3D rotation followed by a 3D translation.
	explicit MMat44(const MMat33& rot, const MVec3& trans);

	// constructs a matrix that represents a 3D rotation.
	explicit MMat44(const MMat33& rot);

	// constructs a matrix that represents a 3D translation.
	explicit MMat44(const MVec3& trans);

	// identity / zero.
	static MMat44	Identity;
	static MMat44	Zero;
	void			SetIdentity()							{ Set(Identity); }
	void			SetZero()								{ Set(Zero); }

	// accessors.
	float			Get(uint y, uint x) const				{ E_ASSERT(y < 4 && x < 4); return _v[4*y + x]; }
	float&			Get(uint y, uint x)						{ E_ASSERT(y < 4 && x < 4); return _v[4*y + x]; }
	float			operator()(uint y, uint x) const		{ E_ASSERT(y < 4 && x < 4); return _v[4*y + x]; }
	float&			operator()(uint y, uint x)				{ E_ASSERT(y < 4 && x < 4); return _v[4*y + x]; }

	// setters.
	void			Set(const MMat44& m);
	MMat44&			operator =(const MMat44& m);

	// computes whether the matrix is orthogonal.
	bool			IsOrtho() const;

	// matrix decomposition (could be slow, use carefully)
	MMat33			GetRot() const;
	void			SetRot(const MMat33& m);
	MVec3			GetTrans() const;
	void			SetTrans(const MVec3& v);

	// transposes the matrix.
	MMat44			CalcTranspose();

	// postmultiplies the matrix.  "A followed by B" is written (B * A)
	MMat44			operator *(const MMat44& m) const;

	// comparison.
	bool			Compare(const MMat44& m, float epsilon) const;
};
//========================================================================
