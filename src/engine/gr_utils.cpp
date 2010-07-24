//========================================================================
//	file:		gr_utils.cpp
//	author:		Shawn Presser 
//
// (c) 2010 Shawn Presser.  All Rights Reserved.
//========================================================================

//========================================================================
// Headers
//========================================================================
#include "e_common.h"
#include "gr_utils.h"
//========================================================================

//===================
// Gr_CalcTangents
//===================
void				Gr_CalcTangents(	SVec3& tangent, SVec3& binormal, const SVec3& p0, const SVec3& p1,
										const SVec3& p2, const SVec2& tc0, const SVec2& tc1, const SVec2& tc2,
										bool normalize )
{
	SVec3 dp0 = p1 - p0;
	SVec3 dp1 = p2 - p0;
	SVec2 dtc0 = tc1 - tc0;
	SVec2 dtc1 = tc2 - tc0;

	E_ASSERT(!FloatZero(dtc0.Mag()));
	E_ASSERT(!FloatZero(dtc1.Mag()));

	// calculate the new tangent and binormal.
	float r = 1.0f / ( dtc0.X() * dtc1.Y() - dtc1.X() * dtc0.Y() );
	tangent = r * ( dtc1.Y() * dp0 - dtc0.Y() * dp1 );
	binormal = r * ( dtc0.X() * dp1 - dtc1.X() * dp0 );
	float tMag = tangent.Mag();
	float bMag = binormal.Mag();

	// normalize if necessary.
	if ( normalize )
	{
		tangent.Normalize();
		binormal.Normalize();
	}
}
