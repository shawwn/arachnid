//========================================================================
//	file:		m_transform.h
//	author:		Shawn Presser 
//	date:		7/13/10
//
// (c) 2010 Shawn Presser.  All Rights Reserved.
//========================================================================
#pragma once

//========================================================================
// Headers
//========================================================================
#include "m_mat33.h"
#include "m_vec3.h"
//========================================================================

//========================================================================
// Declarations
//========================================================================
class MMat44;
class MQuat;
//========================================================================

//========================================================================
// MTransform
//========================================================================
class ENGINE_API MTransform
{
private:
	MMat33			_rot;
	MVec3			_trans;

	MTransform(bool) { /* uninitialized */ }

public:
	//===================================
	// ctors.
	MTransform()															{ SetIdentity(); }
	MTransform(const MTransform& m)											{ Set(m); }

	// represents a 3D rotation without shear followed by a 3D translation.
	explicit MTransform(const MMat33& rot, const MVec3& trans);

	// represents a 3D rotation without shear.
	explicit MTransform(const MMat33& rot);

	// represents a 3D translation.
	explicit MTransform(const MVec3& trans);

	// decomposes M into a 3D rotation without shear followed by a 3D translation.
	explicit MTransform(const MMat44& m);

	// constructs a transform that represents the difference between orientation A and orientation B.
	// That is, given transform A and transform B:
	//		M = FromAtoB(A, B)
	// then (M * A) results in B.
	static MTransform	FromAtoB(const MTransform& A, const MTransform& B);

	// identity / zero.
	static MTransform	Identity;
	void				SetIdentity()										{ Set(Identity); }

	// accessors.
	const MMat33&		GetRot() const										{ return _rot; }
	const MVec3&		GetTrans() const									{ return _trans; }

	// setters.
	void				Set(const MTransform& m);
	MTransform&			operator =(const MTransform& m);

	void				SetRot(const MMat33& rot)							{ _rot = rot; }
	void				SetTrans(const MVec3& trans)						{ _trans = trans; }

	// rotates and translates a point.
	MVec3				RotateTranslate(const MVec3& pt) const;
	void				RotateTranslateXYZ(float& x, float& y, float& z) const;

	// for transforms A and B, (A * B) causes B's rotation and translation to be applied first,
	// followed by A.
	MTransform			operator *(const MTransform& m) const;

	// computes the opposite (inverse) of this transform.
	// since our transform represents "a rotation followed by a translation", then
	// the opposite is "-translation followed by -rotation".
	MTransform			Opposite() const;

	// compares our transform with the specified transform.
	// returns true if they are very close to equivalent (within floating-point roundoff
	// tolerances).
	bool				Compare(const MTransform& m, float epsilon) const;
};
//========================================================================

