//========================================================================
//	file:		m_quat.h
//	author:		Shawn Presser 
//	date:		7/6/10
//
// (c) 2010 Shawn Presser.  All Rights Reserved.
//========================================================================
#pragma once

//========================================================================
// Declarations
//========================================================================
class MMat33;
class MVec3;
//========================================================================

//========================================================================
// MQuat
//========================================================================
class ENGINE_API MQuat
{
private:
	float	_v[4];

public:
	// ctors.
	MQuat()										{ SetIdentity(); }
	MQuat(const MQuat& q)						{ Set(q); }
	MQuat(float w, float x, float y, float z)	{ Set(w, x, y, z); }
	explicit MQuat(const MMat33& mat)			{ *this = FromRotationMatrix(mat); }

	static MQuat	FromRotationMatrix(const MMat33& mat);

	// returns a quaternion that rotates from A to B.
	// In other words, returns the spherical difference between A and B
	// such that A * MQuat::FromAtoB(A, B) results in B.
	static MQuat	FromAtoB(const MQuat& A, const MQuat& B);

	// identity / zero.
	static MQuat	Identity;
	void			SetIdentity()				{ Set(Identity); }

	// accessors.
	float			W() const					{ return _v[0]; }
	float&			W()							{ return _v[0]; }
	float			X() const					{ return _v[1]; }
	float&			X()							{ return _v[1]; }
	float			Y() const					{ return _v[2]; }
	float&			Y()							{ return _v[2]; }
	float			Z() const					{ return _v[3]; }
	float&			Z()							{ return _v[3]; }
	float			Get(uint idx) const			{ E_ASSERT(idx < 4); return _v[idx]; }
	float&			Get(uint idx) 				{ E_ASSERT(idx < 4); return _v[idx]; }
	float			operator()(uint idx) const	{ E_ASSERT(idx < 4); return _v[idx]; }
	float&			operator()(uint idx) 		{ E_ASSERT(idx < 4); return _v[idx]; }

	// setters.
	void			SetW(float v)				{ _v[0] = v; }
	void			SetX(float v)				{ _v[1] = v; }
	void			SetY(float v)				{ _v[2] = v; }
	void			SetZ(float v)				{ _v[3] = v; }
	void			Set(float w, float x, float y, float z);
	void			Set(const MQuat& q);
	MQuat&			operator =(const MQuat& q);

	// vector operations.
	float			MagSqr() const;
	float			Mag() const;
	bool			IsNormalized() const		{ return FloatEqual(Mag(), 1.0f); }

	// normalizes this vector and returns the previous magnitude.
	float			Normalize();

	// converts the quaternion to a rotation matrix.
	MMat33			ToRotationMatrix() const;

	// applies q's rotation to us.
	MQuat			Concat(const MQuat& q) const;
	inline MQuat	Mul(const MQuat& q) const		{ return Concat(q); }

	// computes our inverse, which represents "the opposite of our rotation".
	// In other words, if we represent the axis-angle rotation rot(axis, angle),
	// then our inverse is rot(axis, -angle).
	inline MQuat	Inverse() const				{ return Conjugate(); }
	MQuat			Conjugate() const;

	// Let	A = this quaternion
	//		B = the specified quaternion
	//		T = a value in the range [0.0 .. 1.0]
	//
	// When T is 0.0, the result is A.  When T is 1.0, the result is B.
	// Otherwise, the result is the interpolation from A to B by T.
	//
	// For example, T = 0.25 would result in a quaternion which is one-quarter
	// the way from A to B.
	MQuat			Blend(const MQuat& dest, float t) const;

	// comparison.  Returns true if we are equivalent to the specified quaternion
	// (within floating-point roundoff tolerances).
	bool			Compare(const MQuat& q, float epsilon) const;
};
//========================================================================


