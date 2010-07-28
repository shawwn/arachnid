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
class MTransform;

typedef ushort		TriIdx;
//========================================================================

//========================================================================
// Mesh Channels
//========================================================================
#define MESH_TEXCOORDS		BIT(0)
#define MESH_NORMALS		BIT(1)
#define MESH_TANGENTS		BIT(2)
#define MESH_BINORMALS		BIT(3)
#define MESH_BONE_INFO		BIT(4)
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

	// magnitude.
	float			MagSq() const				{ return SQR(X()) + SQR(Y()); }
	float			Mag() const					{ return SQRT(MagSq()); }

	// normalize.
	float			Normalize();

	// arithmetic operators.
	SVec2						operator +(const SVec2& v) const;
	SVec2&						operator +=(const SVec2& v);
	SVec2						operator -(const SVec2& v) const;
	SVec2&						operator -=(const SVec2& v);
	friend ENGINE_API SVec2		operator *(float s, const SVec2& v);
	SVec2&						operator *=(float s);
};
typedef vector<SVec2> SVec2vec;
//========================================================================

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

	// magnitude.
	float			MagSq() const				{ return SQR(X()) + SQR(Y()) + SQR(Z()); }
	float			Mag() const					{ return SQRT(MagSq()); }

	// normalize.
	float			Normalize();

	// cross.
	SVec3			Cross(const SVec3& v) const;

	// transform.
	void			RotateTranslate(const MTransform& m);

	// arithmetic operators.
	SVec3						operator +(const SVec3& v) const;
	SVec3&						operator +=(const SVec3& v);
	SVec3						operator -(const SVec3& v) const;
	SVec3&						operator -=(const SVec3& v);
	friend ENGINE_API SVec3		operator *(float s, const SVec3& v);
	SVec3&						operator *=(float s);
};
typedef vector<SVec3> SVec3vec;
//========================================================================

//========================================================================
// SVec4
//		guaranteed to be 16 bytes in size.
//========================================================================
struct ENGINE_API SVec4
{
private:
	float		_v[4];

public:
	// ctors.
	SVec4()										{ /* not initialized */ }
	SVec4(const SVec4& v)						{ Set(v); }
	SVec4(const float* p)						{ Set(p); }
	SVec4(float s)								{ Set(s, s, s, s); }
	SVec4(float x, float y, float z, float w)	{ Set(x, y, z, w); }
	SVec4(const SVec3& v, float w)				{ Set(v.X(), v.Y(), v.Z(), w); }

	static SVec4	Zero;
	void			SetZero()					{ Set(Zero); }

	// accessors.
	float			X() const					{ return _v[0]; }
	float&			X()							{ return _v[0]; }
	float			Y() const					{ return _v[1]; }
	float&			Y()							{ return _v[1]; }
	float			Z() const					{ return _v[2]; }
	float&			Z()							{ return _v[2]; }
	float			W() const					{ return _v[3]; }
	float&			W()							{ return _v[3]; }
	const float*	Get() const					{ return _v; }
	float			Get(uint idx) const			{ E_ASSERT(idx < 4); return _v[idx]; }
	float&			Get(uint idx) 				{ E_ASSERT(idx < 4); return _v[idx]; }
	float			operator()(uint idx) const	{ E_ASSERT(idx < 4); return _v[idx]; }
	float&			operator()(uint idx) 		{ E_ASSERT(idx < 4); return _v[idx]; }
	int				GetColor() const;

	// setters.
	void			SetX(float v)				{ _v[0] = v; }
	void			SetY(float v)				{ _v[1] = v; }
	void			SetZ(float v)				{ _v[2] = v; }
	void			SetW(float v)				{ _v[3] = v; }
	void			Set(float x, float y, float z, float w);
	void			Set(const float* p);
	void			Set(const SVec4& v);
	SVec4&			operator =(const SVec4& v);

	// magnitude.
	SVec3			Vec3() const				{ return SVec3(X(), Y(), Z()); }

	// arithmetic operators.
	SVec4						operator +(const SVec4& v) const;
	SVec4&						operator +=(const SVec4& v);
	SVec4						operator -(const SVec4& v) const;
	SVec4&						operator -=(const SVec4& v);
	friend ENGINE_API SVec4		operator *(float s, const SVec4& v);
	SVec4&						operator *=(float s);
};
typedef vector<SVec4> SVec4vec;
//========================================================================

//========================================================================
// SColoredVertex
//========================================================================
struct SColoredVertex
{
	SVec3		pos;
	uint		color;
};

//========================================================================
// SMeshVertex
//========================================================================
struct SMeshVertex
{
	SVec3		position;
	SVec3		normal;
	SVec2		uv;
};
BuildVectorType(SMeshVertexVec, SMeshVertex, 512,
				StoragePolicy::OnCreate::ZeroMem,
				StoragePolicy::OnDestroy::DoNothing,
				StoragePolicy::OnAssign::CopyRawMemory);
//========================================================================

//========================================================================
// EMaterialTexture
//========================================================================
enum EMaterialTexture
{
	MTEX_DIFFUSE,
	MTEX_NORMAL,
	MTEX_SPECULAR,
	MTEX_EMISSIVE,
	MTEX_COUNT
};
//========================================================================

//========================================================================
// EMaterialType
//========================================================================
enum EMaterialType
{
	MAT_SOLID
};

//========================================================================
// EMaterialPass
//========================================================================
enum EMaterialPass
{
	MAT_PASS_AMBIENT,
	MAT_PASS_SHADOW,
	MAT_PASS_LIT,
	MAT_PASS_POSTPROCESS,
	MAT_PASS_COUNT
};
//========================================================================

