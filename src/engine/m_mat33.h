//========================================================================
//	file:		m_mat33.h
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
// MMat33
//		Row-major 3x3 matrix.
//========================================================================
class ENGINE_API MMat33
{
private:
	float	_v[9];

	MMat33(bool) { /* uninitialized */ }

public:
	MMat33()											{ SetIdentity(); }
	MMat33(const MMat33& m)								{ Set(m); }
	MMat33(	float _00,	float _01,	float _02,
			float _10,	float _11,	float _12,
			float _20,	float _21,	float _22 );

	// constructs a matrix that represents a rotation around the X, Y, or Z axis.
	static MMat33	FromXRot(float theta);
	static MMat33	FromYRot(float theta);
	static MMat33	FromZRot(float theta);

	// constructs a matrix that represents a counterclockwise rotation around 'axis'
	// by 'angle' radians.
	static MMat33	FromAxisAngle(const MVec3& axis, float angle);

	// identity / zero.
	static MMat33	Identity;
	static MMat33	Zero;
	void			SetIdentity()						{ Set(Identity); }
	void			SetZero()							{ Set(Zero); }

	// accessors.
	float			Get(int y, int x) const				{ E_ASSERT(x < 3 && y < 3); return _v[3*y + x]; }
	float&			Get(int y, int x)					{ E_ASSERT(x < 3 && y < 3); return _v[3*y + x]; }
	float			operator()(int y, int x) const		{ E_ASSERT(x < 3 && y < 3); return _v[3*y + x]; }
	float&			operator()(int y, int x)			{ E_ASSERT(x < 3 && y < 3); return _v[3*y + x]; }

	void			ToAxisAngle(MVec3& axis, float& angle) const;

	// setters.
	void			Set(const MMat33& m);
	MMat33&			operator =(const MMat33& m);

	// returns whether the matrix has any shearing.
	bool			IsOrtho() const;

	// rotates a point.
	MVec3			RotatePoint(const MVec3& pt) const;
	void			RotatePointXYZ(float& x, float& y, float& z) const;

	// transposes the matrix.
	MMat33			CalcTranspose() const;

	// computes the opposite rotation.
	// the matrix must not have any shearing (must be orthogonal)!
	MMat33			CalcInverseOrtho() const;

	// postmultiplies the matrix.  "A followed by B" is written (B * A)
	MMat33			operator *(const MMat33& m) const;

	// comparison.
	bool			Compare(const MMat33& m, float epsilon) const;
};
//========================================================================
