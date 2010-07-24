//========================================================================
//	file:		gr_camera.cpp
//	author:		Shawn Presser 
//
// (c) 2010 Shawn Presser.  All Rights Reserved.
//========================================================================

//========================================================================
// Headers
//========================================================================
#include "e_common.h"
#include "gr_camera.h"

// math headers.
#include "m_mat33.h"
//========================================================================


//===================
// GrCamera::GrCamera
//===================
GrCamera::GrCamera(const MVec3& side, const MVec3& up, const MVec3& forward)
: _pos(0.0f, 0.0f, 0.0f)
, _side(side)
, _up(up)
, _forward(forward)
{
	_side.Normalize();
	_up.Normalize();
	_forward.Normalize();
}


//===================
// GrCamera::~GrCamera
//===================
GrCamera::~GrCamera()
{
}


//===================
// GrFirstPersonCamera::GrFirstPersonCamera
//===================
GrFirstPersonCamera::GrFirstPersonCamera(const MVec3& worldSide, const MVec3& worldUp, const MVec3& worldForward)
: GrCamera(worldSide, worldUp, worldForward)
, _yaw(0.0f)
, _tilt(0.0f)
{
	_worldSide = _side;
	_worldForward  = _forward;

	// verify that the basis vectors are orthonormal.
	E_ASSERT(FloatZero(_worldSide.Dot(_worldForward)));

	YawTilt(0.0f, 0.0f);
}


//===================
// GrFirstPersonCamera::Impulse
//===================
void			GrFirstPersonCamera::Impulse(const MVec3& sideUpForward)
{
	AddPos(
		sideUpForward.X() * GetSideAxis() +
		sideUpForward.Y() * GetUpAxis() +
		sideUpForward.Z() * GetForwardAxis());
}


//===================
// GrFirstPersonCamera::YawTilt
//===================
void			GrFirstPersonCamera::YawTilt(float dYaw, float dTilt)
{
	dYaw = FRAC(dYaw / M_FULL_TURN) * M_FULL_TURN;

	_yaw += dYaw;
	_tilt += dTilt;

	if (_yaw <= -M_HALF_TURN)
		_yaw += M_FULL_TURN;
	if (_yaw > M_HALF_TURN)
		_yaw -= M_FULL_TURN;

	const float restrictTilt(0.25f * PERCENT_TO_ANGLE(98.0f));
	if (_tilt > restrictTilt)
		_tilt = restrictTilt;
	if (_tilt < -restrictTilt)
		_tilt = -restrictTilt;

	MMat33 tiltRot(MMat33::FromAxisAngle(_worldSide, _tilt));
	MMat33 yawRot(MMat33::FromAxisAngle(_up, _yaw));
	MMat33 finalRot(yawRot * tiltRot);

	_side = finalRot.RotatePoint(_worldSide);
	_forward = finalRot.RotatePoint(_worldForward);
}

