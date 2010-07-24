//========================================================================
//	file:		gr_camera.h
//	author:		Shawn Presser 
//	date:		7/2/10
//
// (c) 2010 Shawn Presser.  All Rights Reserved.
//========================================================================
#pragma once

//========================================================================
// Headers
//========================================================================
#include "m_vec3.h"
#include "m_transform.h"
//========================================================================

//========================================================================
// GrCamera
//========================================================================
class ENGINE_API GrCamera
{
protected:
	MVec3					_pos;

	MVec3					_side;
	MVec3					_up;
	MVec3					_forward;

public:
	GrCamera(const MVec3& side, const MVec3& up, const MVec3& forward);
	virtual ~GrCamera();

	virtual const MVec3&	GetPos() const					{ return _pos; }
	virtual void			SetPos(const MVec3& pos)		{ _pos = pos; }
	virtual void			AddPos(const MVec3& delta)		{ _pos += delta; }

	virtual MVec3			GetSideAxis() const				{ return _side; }
	virtual MVec3			GetUpAxis() const				{ return _up; }
	virtual MVec3			GetForwardAxis() const			{ return _forward; }

	virtual void			GetEyeInfo(MVec3& pos, MVec3& lookAt, MVec3& up) const
	{
		pos = GetPos();
		lookAt = pos + GetForwardAxis();
		up = GetUpAxis();
	}
};
//========================================================================


//========================================================================
// GrFirstPersonCamera
//========================================================================
class ENGINE_API GrFirstPersonCamera : public GrCamera
{
private:
	MVec3					_worldSide;
	MVec3					_worldForward;
	float					_yaw;
	float					_tilt;

public:
	GrFirstPersonCamera(const MVec3& worldSide, const MVec3& worldUp, const MVec3& worldForward);

	virtual MVec3			GetUpAxis() const				{ return _up; }

	virtual void			Impulse(const MVec3& sideUpForward);

	virtual void			YawTilt(float dYaw, float dTilt);
};

