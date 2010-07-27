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
#define M_PI				3.141592653579f
#define M_2PI				6.283185307158f
#define M_HALF_PI			1.57079632267895f;

#define M_FULL_TURN			M_2PI
#define M_HALF_TURN			(0.5f   * M_FULL_TURN)
#define M_QUARTER_TURN		(0.25f  * M_FULL_TURN)
#define M_EIGTH_TURN		(0.125f * M_FULL_TURN)

#define M_ALMOST_ZERO		1e-6
//========================================================================

//========================================================================
// Definitions
//========================================================================

// basic operations.
#define ABS(x)				abs(x)
#define SQR(x)				((x)*(x))
#define SQRT(x)				sqrtf(x)
#define MIN(a, b)			(((a) < (b)) ? (a) : (b))
#define MAX(a, b)			(((a) > (b)) ? (a) : (b))
#define CLAMP(lo, hi, v)	MAX(lo, MIN(hi, v))
#define LERP(lo, hi, t)		((lo) + ((t)*((hi) - (lo))))
#define SATURATE(v)			CLAMP(0, 1, v)
#define FRAC(v)				fmod(v, 1.0f)
#define MOD(val, div)		fmod(val, div)
inline uint FLOOR(float v)	{ return (uint)v; }
inline uint CEIL(float v)	{ return FLOOR(v + 1.0f); }

// vector operations.
#define DOT3(Ax, Ay, Az, Bx, By, Bz) \
	((Ax)*(Bx) + (Ay)*(By) + (Az)*(Bz))

// trig functions.
#define SIN(x)				sinf(x)
#define COS(x)				cosf(x)
#define TAN(x)				tanf(x)
inline void SINCOS(float theta, float& s, float& c)
{
	s = SIN(theta);
	c = COS(theta);
}

#define ASIN(x)				asinf(x)
#define ACOS(x)				acosf(x)
#define ATAN(x)				atanf(x)

#define ATAN2(x, y)			atan2f(x, y)

// conversions.
#define ANGLE_TO_DEGREES(theta)		((180.0f / M_PI) * (theta))
#define DEGREES_TO_ANGLE(deg)		((M_PI / 180.0f) * (deg))

// one turn = 2*PI radians
#define ANGLE_TO_TURNS(theta)		((1.0f / M_FULL_TURN) * (theta))
#define TURNS_TO_ANGLE(turns)		((M_FULL_TURN / 1.0f) * (turns))

// 100% = 2*PI radians
#define ANGLE_TO_PERCENT(theta)		((100.0f / M_FULL_TURN) * (theta))
#define PERCENT_TO_ANGLE(percent)	((M_FULL_TURN / 100.0f) * (percent))

// comparisons.
#define BETWEEN(lo, hi, v)	(((v) >= lo) && ((v) <= hi))
#define NEAR_ZERO(f)		(ABS(f) <= M_ALMOST_ZERO)
//========================================================================

//========================================================================
// Number Utilities
//========================================================================

//===================
// IsPow2
//		returns whether 'x' is a power of two.
//===================
inline bool					IsPow2(uint x)
{
	return (x & (x - 1)) == 0;
}

//===================
// NextPow2
//		returns the next highest power of 2.
//===================
inline uint					NextPow2(uint x)
{
	uint v(1);
	while (x != 0)
	{
		x >>= 1;
		v <<= 1;
	}
	return v;
}

//===================
// FloatZero
//		returns whether 'f' is almost equivalent to zero.
//===================
inline bool					FloatZero(float f, float epsilon = M_ALMOST_ZERO)
{
	if (ABS(f) > (2.0f * epsilon))
		return false;

	return true;
}


//===================
// FloatEqual
//		returns whether 'a' is almost equivalent to 'b'.
//===================
inline bool					FloatEqual(float a, float b, float epsilon = M_ALMOST_ZERO)
{
	if (ABS(a - b) > (2.0f * epsilon))
		return false;

	return true;
}


//===================
// FloatsEqual
//		returns whether 'a' is almost equivalent to 'b'.
//===================
inline bool					FloatsEqual(const float* a, const float* b, uint count, float epsilon = M_ALMOST_ZERO)
{
	for (uint i = 0; i < count; ++i)
	{
		if (!FloatEqual((*a++), (*b++), epsilon))
			return false;
	}
	return true;
}
//========================================================================
