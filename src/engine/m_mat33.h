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
//========================================================================
class MMat33
{
private:
	float	_v[9];

public:
	MMat33()											{ SetIdentity(); }
	MMat33(const MMat33& m)								{ Set(m); }
	MMat33(const float* p)								{ Set(p); }
	MMat33(	float rotXx,	float rotYx,	float rotZx,
			float rotXy,	float rotYy,	float rotZy,
			float rotXz,	float rotYz,	float rotZz );

	static MMat33	Identity;
	static MMat33	Zero;
	void			SetIdentity()						{ Set(Zero); }
	void			SetZero()							{ Set(Zero); }

	// constructs a matrix that represents a rotation around the X, Y, or Z axis.
	static MMat33	XRot(float theta);
	static MMat33	YRot(float theta);
	static MMat33	ZRot(float theta);

	// accessors.
	const float*	Get() const							{ return _v; }
	float			Get(int col, int row) const			{ E_ASSERT(col < 3 && row < 3); return _v[3*col + row]; }
	float&			Get(int col, int row)				{ E_ASSERT(col < 3 && row < 3); return _v[3*col + row]; }
	float			operator()(int col, int row) const	{ E_ASSERT(col < 3 && row < 3); return _v[3*col + row]; }
	float&			operator()(int col, int row)		{ E_ASSERT(col < 3 && row < 3); return _v[3*col + row]; }
	MVec3			GetXAxis() const;
	MVec3			GetYAxis() const;
	MVec3			GetZAxis() const;

	// setters.
	void			Set(const float* p);
	void			Set(const MMat33& m);
	MMat33&			operator =(const MMat33& m);
	void			SetXAxis(const MVec3& v);
	void			SetYAxis(const MVec3& v);
	void			SetZAxis(const MVec3& v);

	// rotates a point.
	MVec3			Rotate(const MVec3& pt) const;
	void			Rotate(MVec3& pt) const;

	// matrix multiplication.
	MMat33			operator *(const MMat33& m) const;
	MMat33&			operator *=(const MMat33& m);
};
//========================================================================
