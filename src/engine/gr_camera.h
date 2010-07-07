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
#include "m_mat33.h"
//========================================================================

//========================================================================
// GrCamera
//========================================================================
class ENGINE_API GrCamera
{
private:
	MVec3		_position;
	MMat33		_rotation;

public:
	GrCamera();
	~GrCamera();

	const MVec3&		GetPosition() const					{ return _position; }
	void				SetPosition(const MVec3& pos)		{ _position = pos; }

	const MMat33&		GetRotation() const					{ return _rotation; }
	void				SetRotation(const MMat33& pos)		{ _rotation = pos; }
};
//========================================================================

