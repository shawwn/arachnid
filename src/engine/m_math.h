//========================================================================
//	file:		m_math.h
//	author:		Shawn Presser 
//	date:		7/1/10
//
// (c) 2010 Shawn Presser.  All Rights Reserved.
//========================================================================
#pragma once

//========================================================================
// Headers
//========================================================================
#include <math.h>
//========================================================================

//========================================================================
// Constants
//========================================================================
#define M_PI			3.141592653579f
#define M_2PI			6.283185307158f

#define M_FULL_TURN		M_2PI
#define M_HALF_TURN		(0.5f   * M_FULL_TURN)
#define M_QUARTER_TURN	(0.25f  * M_FULL_TURN)
#define M_EIGTH_TURN	(0.125f * M_FULL_TURN)

#define M_ALMOST_ZERO	1e-7
//========================================================================

//========================================================================
// Definitions
//========================================================================
#define ABS(x)			abs(x)
#define SQR(x)			((x)*(x))
#define SQRT(x)			sqrtf(x)

// trig functions
#define SIN(x)			sinf(x)
#define COS(x)			cosf(x)
#define TAN(x)			tanf(x)

#define ASIN(x)			asinf(x)
#define ACOS(x)			acosf(x)
#define ATAN(x)			atanf(x)

#define ATAN2(x, y)		atan2f(x, y)

// conversions
#define RAD2DEG(theta)	((180.0f / M_PI) * theta)
#define DEG2RAD(deg)	((M_PI / 180.0f) * deg)
#define NEAR_ZERO(f)	(ABS(f) <= M_ALMOST_ZERO)
//========================================================================

//========================================================================
// Number Utilities
//========================================================================

//===================
// FloatZero
//		returns whether 'f' is almost equivalent to zero.
//===================
inline bool				FloatZero(float f)
{
	return ABS(f) <= M_ALMOST_ZERO;
}

//===================
// FloatZero
//		returns whether 'sqrt(f)' is almost equivalent to zero.
//===================
inline bool				FloatSqrZero(float f)
{
	return ABS(f) <= (M_ALMOST_ZERO * M_ALMOST_ZERO);
}
//========================================================================
