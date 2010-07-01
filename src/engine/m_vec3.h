//========================================================================
//	file:		m_vec3.h
//	author:		Shawn Presser 
//	date:		6/30/10
//
// (c) 2010 Shawn Presser.  All Rights Reserved.
//========================================================================
#pragma once

//========================================================================
// MVec3
//========================================================================
class MVec3
{
private:
	float	_v[3];

public:
	MVec3();
	MVec3(float s);
	MVec3(float x, float y, float z);
	MVec3(const float* p);

	float			X() const		{ return _v[0]; }
	float			Y() const		{ return _v[1]; }
	float			Z() const		{ return _v[2]; }
	const float*	Get() const		{ return _v; }

	void			SetX(float v)	{ _v[0] = v; }
	void			SetY(float v)	{ _v[1] = v; }
	void			SetZ(float v)	{ _v[2] = v; }
	void			Set(const float* p)	{ SetX(p[0]); SetY(p[1]); SetZ(p[2]); }

	float			MagSqr() const;
	float			Mag() const;
	float			Dot(const MVec3& v);

	MVec3			Dir(const MVec3& point);

	MVec3			Cross(const MVec3& axis);

	MVec3			operator +(const MVec3& v) const;
	MVec3&			operator +=(const MVec3& v);

	MVec3			operator -(const MVec3& v) const;
	MVec3&			operator -=(const MVec3& v);

	friend MVec3	operator *(float s, const MVec3& v);
	MVec3&			operator *=(float s);
};
//========================================================================


