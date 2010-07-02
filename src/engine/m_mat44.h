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
//========================================================================
class MMat44
{
private:
	float	_v[16];

public:
	// ctors.
	MMat44()												{ SetIdentity(); }
	MMat44(const MMat44& m)									{ Set(m); }
	MMat44(const float* p)									{ Set(p); }
	MMat44(	float rotXx,	float rotYx,	float rotZx,	float Wx,
			float rotXy,	float rotYy,	float rotZy,	float Wy,
			float rotXz,	float rotYz,	float rotZz,	float Wz,
			float Tx,		float Ty,		float Tz,		float Ww);

	static MMat44	Identity;
	static MMat44	Zero;
	void			SetIdentity()							{ Set(Identity); }
	void			SetZero()								{ Set(Zero); }

	// accessors.
	const float*	Get() const								{ return _v; }
	float			Get(uint col, uint row) const			{ E_ASSERT(col < 4 && row < 4); return _v[4*col + row]; }
	float&			Get(uint col, uint row)					{ E_ASSERT(col < 4 && row < 4); return _v[4*col + row]; }
	float			operator()(uint col, uint row) const	{ E_ASSERT(col < 4 && row < 4); return _v[4*col + row]; }
	float&			operator()(uint col, uint row)			{ E_ASSERT(col < 4 && row < 4); return _v[4*col + row]; }

	// setters.
	void			Set(const float* p);
	void			Set(const MMat44& m);
	MMat44&			operator =(const MMat44& m);

	// matrix decomposition.
	MVec3			GetTransform() const;
	void			SetTransform(const MVec3& t);

	MMat33			GetRot() const;
	void			SetRot(const MMat33& r);

	// rotates and translates the point.
	MVec3			RotateTranslate(const MVec3& pt);
	void			RotateTranslate(MVec3& pt);

	// matrix multiplication.
	MMat44			operator *(const MMat44& m) const;
	MMat44&			operator *=(const MMat44& m);
};
//========================================================================
