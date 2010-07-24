//========================================================================
//	file:		m_quat.cpp
//	author:		Shawn Presser 
//
// (c) 2010 Shawn Presser.  All Rights Reserved.
//========================================================================

//========================================================================
// Headers
//========================================================================
#include "e_common.h"
#include "m_quat.h"

// math headers.
#include "m_mat33.h"
#include "m_vec3.h"
//========================================================================

//========================================================================
// Constants
//========================================================================
#define QUAT_SIZE				(sizeof(float) * 4)
//========================================================================

//========================================================================
// Definitions
//========================================================================
MQuat	MQuat::Identity(1.0f, 0.0f, 0.0f, 0.0f);
//========================================================================

//========================================================================
// Macros
//========================================================================
// quaternion multiplication.
#define QMUL_W(Aw, Ax, Ay, Az,  Bw, Bx, By, Bz)		((Aw)*(Bw) - (Ax)*(Bx) - (Ay)*(By) - (Az)*(Bz))
#define QMUL_X(Aw, Ax, Ay, Az,  Bw, Bx, By, Bz)		((Aw)*(Bx) + (Ax)*(Bw) + (Ay)*(Bz) - (Az)*(By))
#define QMUL_Y(Aw, Ax, Ay, Az,  Bw, Bx, By, Bz)		((Aw)*(By) - (Ax)*(Bz) + (Ay)*(Bw) + (Az)*(Bx))
#define QMUL_Z(Aw, Ax, Ay, Az,  Bw, Bx, By, Bz)		((Aw)*(Bz) + (Ax)*(By) - (Ay)*(Bx) + (Az)*(Bw))
//========================================================================


//===================
// MQuat::FromRotationMatrix
//===================
MQuat MQuat::FromRotationMatrix(const MMat33& m)
{
	float w;
	float x;
	float y;
	float z;

	float trace = m(0, 0) + m(1, 1) + m(2, 2);
	if (trace > 0.0f)
	{
		float s = 0.5f / sqrtf(trace + 1.0f);
		w = 0.25f / s;
		x = (m(2, 1) - m(1, 2)) * s;
		y = (m(0, 2) - m(2, 0)) * s;
		z = (m(1, 0) - m(0, 1)) * s;
	}
	else
	{
		if (m(0, 0) > m(1, 1) && m(0, 0) > m(2, 2))
		{
			float s = 2.0f * sqrtf(1.0f + m(0, 0) - m(1, 1) - m(2, 2));
			w = (m(2, 1) - m(1, 2)) / s;
			x = 0.25f * s;
			y = (m(0, 1) + m(1, 0)) / s;
			z = (m(0, 2) + m(2, 0)) / s;
		}
		else if (m(1, 1) > m(2, 2))
		{
			float s = 2.0f * sqrtf(1.0f + m(1, 1) - m(0, 0) - m(2, 2));
			w = (m(0, 2) - m(2, 0)) / s;
			x = (m(0, 1) + m(1, 0)) / s;
			y = 0.25f * s;
			z = (m(1, 2) + m(2, 1)) / s;
		}
		else
		{
			float s = 2.0f * sqrtf(1.0f + m(2, 2) - m(0, 0) - m(1, 1));
			w = (m(1, 0) - m(0, 1)) / s;
			x = (m(0, 2) + m(2, 0)) / s;
			y = (m(1, 2) + m(2, 1)) / s;
			z = 0.25f * s;
		}
	}

	return MQuat(w, x, y, z);
}


//===================
// MQuat::FromAtoB
//===================
MQuat			MQuat::FromAtoB(const MQuat& A, const MQuat& B)
{
	// given the quaternions A and B, then the result of (~A) * B
	// is a quaternion which will rotate from A to B.
	MQuat r = A.Conjugate().Mul(B);
	E_ASSERT(r.IsNormalized());
	return r;
}


//===================
// MQuat::Set
//===================
void			MQuat::Set(float w, float x, float y, float z)
{
	SetW(w);
	SetX(x);
	SetY(y);
	SetZ(z);
}


//===================
// MQuat::Set
//===================
void			MQuat::Set(const MQuat& v)
{
	MemCpy(_v, v._v, QUAT_SIZE);
}


//===================
// MQuat::Set
//===================
MQuat&			MQuat::operator =(const MQuat& v)
{
	MemCpy(_v, v._v, QUAT_SIZE);
	return *this;
}


//===================
// MQuat::MagSqr
//===================
float			MQuat::MagSqr() const
{
	return SQR(W()) + SQR(X()) + SQR(Y()) + SQR(Z());
}


//===================
// MQuat::Mag
//===================
float			MQuat::Mag() const
{
	return SQRT(MagSqr());
}


//===================
// MQuat::Normalize
//===================
float			MQuat::Normalize()
{
	// compute the current magnitude.
	float mag(Mag());

	// verify that the magnitude is not zero.
	E_ASSERT(!FloatZero(mag));

	// divide by the magnitude.
	float invMag(1.0f / mag);
	W() *= invMag;
	X() *= invMag;
	Y() *= invMag;
	Z() *= invMag;

	// return the previous magnitude.
	return mag;
}


//===================
// MQuat::ToRotationMatrix
//===================
MMat33			MQuat::ToRotationMatrix() const
{
	float mag(Mag());
	E_ASSERT(!FloatZero(mag));
	float invMag(1.0f / mag);

	float x = X() * invMag;
	float y = Y() * invMag;
	float z = Z() * invMag;
	float w = W() * invMag;

#if 1
	// calculate coefficients
	float x2(x + x ), y2(y + y ), z2(z + z );
	float xx(x * x2), xy(x * y2), xz(x * z2);
	float yy(y * y2), yz(y * z2), zz(z * z2);
	float wx(w * x2), wy(w * y2), wz(w * z2);

	MMat33 r;
	r(0, 0) = 1.0f - (yy + zz);
	r(0, 1) = xy - wz;
	r(0, 2) = xz + wy;		

	r(1, 0) = xy + wz;
	r(1, 1) = 1.0f - (xx + zz);
	r(1, 2) = yz - wx;		

	r(2, 0) = xz - wy;
	r(2, 1) = yz + wx;
	r(2, 2) = 1.0f - (xx + yy);
	return r;
#else
	float x2 = x + x;
	float y2 = y + y;
	float z2 = z + z;
	float xx = x * x2;
	float xy = x * y2;
	float xz = x * z2;
	float yy = y * y2;
	float yz = y * z2;
	float zz = z * z2;
	float wx = w * x2;
	float wy = w * y2;
	float wz = w * z2;

	// build our matrix and return it.
	MMat33 r;
	r(0, 0) = 1.0f - (yy + zz);
	r(0, 1) = xy - wz;
	r(0, 2) = xz + wy;

	r(1, 0) = xy + wz;
	r(1, 1) = 1.0f - (xx + zz);
	r(1, 2) = yz - wx;
	
	r(2, 0) = xz - wy;
	r(2, 1) = yz + wx;
	r(2, 2) = 1.0f - (xx + yy);
	return r;
#endif
}


//===================
// MQuat::Concat
//===================
MQuat			MQuat::Concat(const MQuat& q) const
{
#if 1
	const float Aw(W()), Ax(X()), Ay(Y()), Az(Z());
	const float Bw(q.W()), Bx(q.X()), By(q.Y()), Bz(q.Z());

	return MQuat(
		QMUL_W(Aw, Ax, Ay, Az,	Bw, Bx, By, Bz),
		QMUL_X(Aw, Ax, Ay, Az,	Bw, Bx, By, Bz),
		QMUL_Y(Aw, Ax, Ay, Az,	Bw, Bx, By, Bz),
		QMUL_Z(Aw, Ax, Ay, Az,	Bw, Bx, By, Bz));
#else
	// w*w' - dot(v, v');
	float w = W()*q.W() - ( X()*q.X() + Y()*q.Y() + Z()*q.Z() );

	// v.Cross( v' ) + wv' + w'v;
	float x = Y()*q.Z() - Z()*q.Y();
	float y = Z()*q.X() - X()*q.Z();
	float z = X()*q.Y() - Y()*q.X();
	x += W() * q.X();
	x += q.W() * X();
	y += W() * q.Y();
	y += q.W() * Y();
	z += W() * q.Z();
	z += q.W() * Z();

	// return our new quaternion.
	return MQuat( w, x, y, z );
#endif
}


//===================
// MQuat::Conjugate
//===================
MQuat			MQuat::Conjugate() const
{
	return MQuat(W(), -X(), -Y(), -Z());
}


//===================
// MQuat::Blend
//===================
MQuat			MQuat::Blend(const MQuat& dest, float t) const
{
	const MQuat& A(*this);
	const MQuat& B(dest);

	if (t <= 0.0f)
		return A;

	if (t >= 1.0f)
		return B;

	E_ASSERT(A.IsNormalized() && B.IsNormalized());

#if 1
	float fScale0, fScale1;

	// calc cosine
	float fCosOmega(A.X() * B.X() + A.Y() * B.Y() + A.Z() * B.Z() + A.W() * B.W());

	float fAbsCosOmega(fabs(fCosOmega));
	
	const float QUAT_EPSILON = 1.19209e-007f;
	if ((1.0f - fAbsCosOmega) > QUAT_EPSILON)
	{
		float fSinSqr(1.0f - fAbsCosOmega * fAbsCosOmega);
		
		float fSinOmega(1.0f / sqrt(fSinSqr));
		float fOmega(atan2(fSinSqr * fSinOmega, fAbsCosOmega));

		fScale0 = sin((1.0f - t) * fOmega) * fSinOmega;
		fScale1 = sin(t * fOmega) * fSinOmega;
	}
	else
	{
		fScale0 = 1.0f - t;
		fScale1 = t;
	}
	
	// Adjust sign
	fScale1 = (fCosOmega >= 0.0f) ? fScale1 : -fScale1;

	return MQuat(
		fScale0 * A.W() + fScale1 * B.W(),
		fScale0 * A.X() + fScale1 * B.X(),
		fScale0 * A.Y() + fScale1 * B.Y(),
		fScale0 * A.Z() + fScale1 * B.Z());
#else
#if 0
	// compute a quaternion representing "rotation from A to B"
	MQuat AtoB(FromAtoB(A, B));

	// convert the result into a rotation axis and a rotation angle.
	MVec3 AtoB_axis;
	float AtoB_angle;
	if (!AtoB.ToAxisAngle(AtoB_axis, AtoB_angle))
		return A;

	// now simply scale the angle.
	// When t == 0.0, the angle becomes zero, so the result is identity.
	// When t == 1.0, the angle is not modified, so the result is a full rotation from A to B.
	// when t == 0.5, the angle is halved, so the result is half of a full rotation from A to B.
	MQuat AtoB_interp(FromAxisAngle(AtoB_axis, t * AtoB_angle));

	// since A * AtoB = B, then A * AtoB_interp is our answer.
	return A.Concat(AtoB_interp);
#else
	float dest1[4];
	float omega, cosom, sinom, scale0, scale1;

	// calc cosine
	cosom = X() * dest.X() + Y() * dest.Y() + Z() * dest.Z() + W() * dest.W();

	// adjust signs (if necessary)
	if ( cosom < 0.0 )
	{
		cosom = -cosom;
		dest1[0] = -dest.W();
		dest1[1] = -dest.X();
		dest1[2] = -dest.Y();
		dest1[3] = -dest.Z();
	}
	else
	{
		dest1[0] = dest.W();
		dest1[1] = dest.X();
		dest1[2] = dest.Y();
		dest1[3] = dest.Z();
	}

	// calculate coefficients
	if ( (1.0 - cosom) > 0.001f )
	{
		// standard case (slerp)
		omega = ACOS( cosom );
		sinom = SIN( omega );
		scale0 = SIN( ( 1.0f - t ) * omega ) / sinom;
		scale1 = SIN( t * omega ) / sinom;
	}
	else
	{
		// "from" and "dest" quaternions are very close 
		//  ... so we can do a linear interpolation
		scale0 = 1.0f - t;
		scale1 = t;
	}

	// calculate final values
	return MQuat(
		scale0 * W() + scale1 * dest1[0],
		scale0 * X() + scale1 * dest1[1],
		scale0 * Y() + scale1 * dest1[2],
		scale0 * Z() + scale1 * dest1[3]);
#endif
#endif
}


//===================
// MQuat::Compare
//===================
bool			MQuat::Compare(const MQuat& q, float epsilon) const
{
	MQuat A(*this);
	MQuat B(q);
	A.Normalize();
	B.Normalize();

	for (int i = 0; i < 4; ++i)
	{
		if (!(FloatEqual(A(i), B(i), epsilon)))
			return false;
	}

	return true;
}

