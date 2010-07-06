//========================================================================
//	file:		gr_types.h
//	author:		Shawn Presser 
//	date:		7/6/10
//
// (c) 2010 Shawn Presser.  All Rights Reserved.
//========================================================================
#pragma once

//========================================================================
// Declarations
//========================================================================
class MMat44;

typedef ushort		TriIdx;
//========================================================================

//========================================================================
// SVec2
//		guaranteed to be 8 bytes in size.
//========================================================================
struct ENGINE_API SVec2
{
private:
	float		_v[2];

public:
	// ctors.
	SVec2()										{ /* not initialized */ }
	SVec2(const SVec2& v)						{ Set(v); }
	SVec2(const float* p)						{ Set(p); }
	SVec2(float s)								{ Set(s, s); }
	SVec2(float x, float y)						{ Set(x, y); }

	static SVec2	Zero;
	void			SetZero()					{ Set(Zero); }

	// accessors.
	float			X() const					{ return _v[0]; }
	float&			X()							{ return _v[0]; }
	float			Y() const					{ return _v[1]; }
	float&			Y()							{ return _v[1]; }
	const float*	Get() const					{ return _v; }
	float			Get(uint idx) const			{ E_ASSERT(idx < 2); return _v[idx]; }
	float&			Get(uint idx) 				{ E_ASSERT(idx < 2); return _v[idx]; }
	float			operator()(uint idx) const	{ E_ASSERT(idx < 2); return _v[idx]; }
	float&			operator()(uint idx) 		{ E_ASSERT(idx < 2); return _v[idx]; }

	// setters.
	void			SetX(float v)				{ _v[0] = v; }
	void			SetY(float v)				{ _v[1] = v; }
	void			Set(float x, float y);
	void			Set(const float* p);
	void			Set(const SVec2& v);
	SVec2&			operator =(const SVec2& v);
};

//========================================================================
// SVec3
//		guaranteed to be 12 bytes in size.
//========================================================================
struct ENGINE_API SVec3
{
private:
	float		_v[3];

public:
	// ctors.
	SVec3()										{ /* not initialized */ }
	SVec3(const SVec3& v)						{ Set(v); }
	SVec3(const float* p)						{ Set(p); }
	SVec3(float s)								{ Set(s, s, s); }
	SVec3(float x, float y, float z)			{ Set(x, y, z); }

	static SVec3	Zero;
	void			SetZero()					{ Set(Zero); }

	// accessors.
	float			X() const					{ return _v[0]; }
	float&			X()							{ return _v[0]; }
	float			Y() const					{ return _v[1]; }
	float&			Y()							{ return _v[1]; }
	float			Z() const					{ return _v[2]; }
	float&			Z()							{ return _v[2]; }
	const float*	Get() const					{ return _v; }
	float			Get(uint idx) const			{ E_ASSERT(idx < 3); return _v[idx]; }
	float&			Get(uint idx) 				{ E_ASSERT(idx < 3); return _v[idx]; }
	float			operator()(uint idx) const	{ E_ASSERT(idx < 3); return _v[idx]; }
	float&			operator()(uint idx) 		{ E_ASSERT(idx < 3); return _v[idx]; }

	// setters.
	void			SetX(float v)				{ _v[0] = v; }
	void			SetY(float v)				{ _v[1] = v; }
	void			SetZ(float v)				{ _v[2] = v; }
	void			Set(float x, float y, float z);
	void			Set(const float* p);
	void			Set(const SVec3& v);
	SVec3&			operator =(const SVec3& v);

	void			RotateTranslate(const MMat44& m);
};
//========================================================================



