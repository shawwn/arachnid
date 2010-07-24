//========================================================================
//	file:		m_transform.cpp
//	author:		Shawn Presser 
//
// (c) 2010 Shawn Presser.  All Rights Reserved.
//========================================================================

//========================================================================
// Headers
//========================================================================
#include "e_common.h"
#include "m_transform.h"

// math headers.
#include "m_mat44.h"
#include "m_quat.h"
//========================================================================

//========================================================================
// Definitions
//========================================================================
#define UNINITIALIZED_TRANSFORM			true
//========================================================================

//========================================================================
// Definitions
//========================================================================
MTransform	MTransform::Identity(MMat33::Identity, MVec3::Zero);
//========================================================================


//===================
// MTransform::MTransform(rot, trans)
//===================
MTransform::MTransform(const MMat33& rot, const MVec3& trans)
{
	SetRot(rot);
	SetTrans(trans);
}


//===================
// MTransform::MTransform(rot)
//===================
MTransform::MTransform(const MMat33& rot)
{
	SetRot(rot);
	SetTrans(MVec3::Zero);
}


//===================
// MTransform::MTransform(trans)
//===================
MTransform::MTransform(const MVec3& trans)
{
	SetTrans(trans);
}


//===================
// MTransform::MTransform(mat44)
//===================
MTransform::MTransform(const MMat44& mat)
{
	E_ASSERT(mat.IsOrtho());
	SetRot(mat.GetRot());
	SetTrans(mat.GetTrans());
}


//===================
// MTransform::FromAtoB
//===================
MTransform			MTransform::FromAtoB(const MTransform& A, const MTransform& B)
{
	MTransform r(UNINITIALIZED_TRANSFORM);

	// the rotation is the difference from A to B.
	r._rot = MQuat::FromAtoB(MQuat(A.GetRot()), MQuat(B.GetRot())).ToRotationMatrix();

	// the translation is the difference from A to B.
	r._trans = B._trans - A._trans;
	return r;
}


//===================
// MTransform::Set
//===================
void				MTransform::Set(const MTransform& m)
{
	_rot.Set(m._rot);
	_trans.Set(m._trans);
}


//===================
// MTransform::operator =
//===================
MTransform&			MTransform::operator =(const MTransform& m)
{
	_rot.Set(m._rot);
	_trans.Set(m._trans);
	return *this;
}


//===================
// MTransform::RotateTranslate
//===================
MVec3				MTransform::RotateTranslate(const MVec3& pt) const
{
	return _rot.RotatePoint(pt) + _trans;
}


//===================
// MTransform::RotateTranslateXYZ
//===================
void				MTransform::RotateTranslateXYZ(float& x, float& y, float& z) const
{
	_rot.RotatePointXYZ(x, y, z);

	x += _trans.X();
	y += _trans.Y();
	z += _trans.Z();
}


//===================
// MTransform::operator *
//===================
MTransform			MTransform::operator *(const MTransform& m) const
{
	MTransform r(UNINITIALIZED_TRANSFORM);
	r._rot = _rot * m._rot;
	r._trans = _rot.RotatePoint(m._trans) + _trans;
	return r;
}


//===================
// MTransform::Opposite
//===================
MTransform			MTransform::Opposite() const
{
	// since our transform represents "a rotation followed by a translation", then
	// the opposite is "-translation followed by -rotation".
	MTransform negTrans(-_trans);
	MTransform negRot(_rot.CalcInverseOrtho());
	return (negRot * negTrans);
}


//===================
// MTransform::Compare
//===================
bool				MTransform::Compare(const MTransform& m, float epsilon) const
{
	if (!(_rot.Compare(m._rot, epsilon)))
		return false;

	if (!_trans.Compare(m._trans, epsilon))
		return false;

	return true;
}


