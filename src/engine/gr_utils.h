//========================================================================
//	file:		gr_utils.h
//	author:		Shawn Presser 
//	date:		7/21/30
//
// (c) 2010 Shawn Presser.  All Rights Reserved.
//========================================================================
#pragma once

//========================================================================
// Utilities
//========================================================================
extern ENGINE_API void		Gr_CalcTangents(	SVec3& tangent, SVec3& binormal, const SVec3& p0, const SVec3& p1,
												const SVec3& p2, const SVec2& tc0, const SVec2& tc1, const SVec2& tc2,
												bool normalize );
//========================================================================
