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
	MMat33();
	MMat33(	float _00, float _10, float _20,
			float _01, float _11, float _21,
			float _02, float _12, float _22 );
	MMat33(const float* p)							{ Set(p); }

	static MMat33	IdentityMat;
	void			Identity();

	// functions to construct rotation matrices.
	static MMat33	RotX(float theta);
	static MMat33	RotY(float theta);
	static MMat33	RotZ(float theta);

	MVec3			Rotate(const MVec3& v);
	void			Rotate(MVec3& v);

	const float*	Get() const						{ return _v; }
	void			Set(const float* p);

	float			Get(int col, int row) const		{ return (*this)(col, row); }
	float&			Get(int col, int row)			{ return (*this)(col, row); }

	MVec3			GetXAxis() const;
	MVec3			GetYAxis() const;
	MVec3			GetZAxis() const;

	void			SetXAxis(const MVec3& v);
	void			SetYAxis(const MVec3& v);
	void			SetZAxis(const MVec3& v);

	float			operator()(int col, int row) const;
	float&			operator()(int col, int row);

	MMat33			operator *(const MMat33& m);
	MMat33&			operator *=(const MMat33& m);
};
//========================================================================
