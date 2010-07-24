//========================================================================
//	file:		m_color.cpp
//	author:		Shawn Presser 
//
// (c) 2010 Shawn Presser.  All Rights Reserved.
//========================================================================

//========================================================================
// Headers
//========================================================================
#include "e_common.h"
#include "m_color.h"
//========================================================================


//===================
// MColor::MColor
//===================
MColor::MColor()
{
	SetR(0.0f);
	SetG(0.0f);
	SetB(0.0f);
	SetA(1.0f);
}


//===================
// MColor::MColor
//===================
MColor::MColor(float r, float g, float b, float a)
{
	SetR(r);
	SetG(g);
	SetB(b);
	SetA(a);
}


//===================
// MColor::MColor
//===================
MColor::MColor(const float* p)
{
	Set(p);
}


//===================
// MColor::MColor
//===================
MColor			MColor::ModulateRGB(const MColor& rgb)
{
	return MColor(R() * rgb.R(),  G() * rgb.G(), B() * rgb.B(), A());
}

