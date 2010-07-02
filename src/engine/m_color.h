//========================================================================
//	file:		m_color.h
//	author:		Shawn Presser 
//	date:		6/30/10
//
// (c) 2010 Shawn Presser.  All Rights Reserved.
//========================================================================
#pragma once

//========================================================================
// MColor
//========================================================================
class MColor
{
private:
	float	_v[4];

public:
	MColor();
	MColor(float r, float g, float b, float a);
	MColor(const float* p);

	float			R() const		{ return _v[0]; }
	float			G() const		{ return _v[1]; }
	float			B() const		{ return _v[2]; }
	float			A() const		{ return _v[3]; }
	const float*	Get() const		{ return _v; }

	void			SetR(float v)	{ _v[0] = v; }
	void			SetG(float v)	{ _v[1] = v; }
	void			SetB(float v)	{ _v[2] = v; }
	void			SetA(float v)	{ _v[3] = v; }
	void			Set(const float* p)	{ SetR(p[0]); SetG(p[1]); SetB(p[2]); SetA(p[3]); }

	MColor			ModulateRGB(const MColor& rgb);
};
//========================================================================


