//========================================================================
//	file:		e_string_converters.cpp
//	author:		Shawn Presser 
//
// (c) 2010 Shawn Presser.  All Rights Reserved.
//========================================================================

//========================================================================
// Headers
//========================================================================
#include "e_common.h"
#include "e_string_converters.h"

// math headers.
#include "m_vec3.h"
#include "m_quat.h"
#include "m_mat33.h"
#include "m_mat44.h"
#include "m_transform.h"
//========================================================================

//========================================================================
// Constants
//========================================================================
#define ROT_FIELD_SIZE				4
//========================================================================

//===================
// TO_STR(bool)
//===================
const wstring&			TO_STR(bool b)
{
	if (b)
	{
		static const wstring strTrue(_T("true"));
		return strTrue;
	}
	else
	{
		static const wstring strFalse(_T("false"));
		return strFalse;
	}
}
const string&			TO_STR_A(bool b)
{
	if (b)
	{
		static const string strTrue("true");
		return strTrue;
	}
	else
	{
		static const string strFalse("false");
		return strFalse;
	}
}


//===================
// TO_STR(int)
//===================
wstring					TO_STR(int i)
{
	wchar_t buf[2048];
	PRINT_STR_BUF(buf, 2048, _T("%d"), i);
	return buf;
}


//===================
// TO_STR(uint)
//===================
wstring					TO_STR(uint i)
{
	wchar_t buf[2048];
	PRINT_STR_BUF(buf, 2048, _T("%u"), i);
	return buf;
}


//===================
// TO_STR(float)
//===================
wstring					TO_STR(float f, uint decimals)
{
	E_ASSERT(decimals < MAX_DECIMALS);
	wchar_t fmt[32];
	PRINT_STR_BUF(fmt, 32, _T("%%.%df"), decimals);

	wchar_t buf[2048];
	PRINT_STR_BUF(buf, 2048, fmt, f);
	return buf;
}


//===================
// TO_STR(MVec3)
//===================
wstring					TO_STR(const MVec3& v, uint decimals)
{
	E_ASSERT(decimals < MAX_DECIMALS);
	wchar_t fmt[32];
	PRINT_STR_BUF(fmt, 32, _T("%%.%df"), decimals);

	wchar_t fmt2[3*32];
	PRINT_STR_BUF(fmt2, 3*32, _T("(%s %s %s)"), fmt, fmt, fmt);

	wchar_t buf[2048];
	PRINT_STR_BUF(buf, 2048, fmt2, v.X(), v.Y(), v.Z());
	return buf;
}


//===================
// TO_STR(MQuat)
//===================
wstring					TO_STR(const MQuat& q, uint decimals)
{
#if TODO
	E_ASSERT(rotDecimals < MAX_DECIMALS);
	// convert rotation to (yaw, pitch, roll).
	float yaw, pitch, roll;
	q.ToRotationMatrix().ToAngles(yaw, pitch, roll);
	yaw		= ANGLE_TO_PERCENT(yaw);
	pitch	= ANGLE_TO_PERCENT(pitch);
	roll	= ANGLE_TO_PERCENT(roll);

	wchar_t fmt[32];
	PRINT_STR_BUF(fmt, 32, _T("%%%d.%df%%%%"), ROT_FIELD_SIZE + rotDecimals, rotDecimals);

	wchar_t fmt2[3*32];
	PRINT_STR_BUF(fmt2, 3*32, _T("rot(%s,%s,%s)"), fmt, fmt, fmt);

	wchar_t buf[2048];
	PRINT_STR_BUF(buf, 2048, fmt2, yaw, pitch, roll);
	return buf;
#else
	E_ASSERT(decimals < MAX_DECIMALS);

	wchar_t fmt[32];
	PRINT_STR_BUF(fmt, 32, _T("%%.%df"), decimals);

	wchar_t fmt2[4*32];
	PRINT_STR_BUF(fmt2, 3*32, _T("quat(%s (%s %s %s))"), fmt, fmt, fmt, fmt);

	wchar_t buf[2048];
	PRINT_STR_BUF(buf, 2048, fmt2, q.W(), q.X(), q.Y(), q.Z());
	return buf;
#endif
}


//===================
// TO_STR(MMat33)
//===================
wstring					TO_STR(const MMat33& m, uint rotDecimals, uint decimals)
{
	E_ASSERT(rotDecimals < MAX_DECIMALS);
	E_ASSERT(decimals < MAX_DECIMALS);

#if TODO
	// for ortho matrices, print rotation angles.  Otherwise print out
	// each of the 3 column vectors.
	if (m.IsOrtho())
	{
		// convert rotation to (yaw, pitch, roll).
		float yaw, pitch, roll;
		m.ToAngles(yaw, pitch, roll);
		yaw		= ANGLE_TO_PERCENT(yaw);
		pitch	= ANGLE_TO_PERCENT(pitch);
		roll	= ANGLE_TO_PERCENT(roll);

		wchar_t fmt[32];
		PRINT_STR_BUF(fmt, 32, _T("%%%d.%df%%%%"), ROT_FIELD_SIZE + rotDecimals, rotDecimals);

		wchar_t fmt2[3*32];
		PRINT_STR_BUF(fmt2, 3*32, _T("rot(%s,%s,%s)"), fmt, fmt, fmt);

		wchar_t buf[2048];
		PRINT_STR_BUF(buf, 2048, fmt2, yaw, pitch, roll);
		return buf;
	}
	else
#endif
	{
		wchar_t fmt[32];
		PRINT_STR_BUF(fmt, 32, _T("%%.%df"), decimals);

		wchar_t fmt2[3*32];
		PRINT_STR_BUF(fmt2, 3*32, _T("%s %s %s"), fmt, fmt, fmt);

		wchar_t fmt3[3*3*32];
		PRINT_STR_BUF(fmt3, 3*3*32, _T("[row0(%s) row1(%s) row2(%s)]"), fmt2, fmt2, fmt2);

		wchar_t buf[2048];
		PRINT_STR_BUF(buf, 2048, fmt3,
			m(0, 0), m(0, 1), m(0, 2),
			m(1, 0), m(1, 1), m(1, 2),
			m(2, 0), m(2, 1), m(2, 2));
		return buf;
	}
}


//===================
// TO_STR(MMat44)
//===================
wstring					TO_STR(const MMat44& m, uint rotDecimals, uint transDecimals, uint decimals)
{
	E_ASSERT(rotDecimals < MAX_DECIMALS);
	E_ASSERT(transDecimals < MAX_DECIMALS);
	E_ASSERT(decimals < MAX_DECIMALS);

#if TODO
	// for ortho matrices, print rot-trans.  Otherwise print out
	// each of the 4 basis vectors.
	if (m.IsOrtho())
	{
		// convert rotation to (yaw, pitch, roll).
		float yaw, pitch, roll;
		m.GetRot().ToAngles(yaw, pitch, roll);
		yaw		= ANGLE_TO_PERCENT(yaw);
		pitch	= ANGLE_TO_PERCENT(pitch);
		roll	= ANGLE_TO_PERCENT(roll);

		// get the translation component.
		MVec3 trans(m.GetTrans());

		wchar_t rotFmt[32];
		PRINT_STR_BUF(rotFmt, 32, _T("%%%d.%df%%%%"), ROT_FIELD_SIZE + rotDecimals, rotDecimals);

		wchar_t fmt[32];
		PRINT_STR_BUF(fmt, 32, _T("%%.%df"), transDecimals);

		wchar_t fmt2[6*32];
		PRINT_STR_BUF(fmt2, 6*32, _T("rot(%s,%s,%s) trans(%s,%s,%s)"),
			rotFmt, rotFmt, rotFmt,
			fmt, fmt, fmt);

		wchar_t buf[2048];
		PRINT_STR_BUF(buf, 2048, fmt2,
			yaw, pitch, roll,
			trans.X(), trans.Y(), trans.Z());
		return buf;
	}
	else
#endif
	{
		wchar_t fmt[32];
		PRINT_STR_BUF(fmt, 32, _T("%%.%df"), decimals);

		wchar_t fmt2[4*32];
		PRINT_STR_BUF(fmt2, 4*32, _T("%s %s %s %s"), fmt, fmt, fmt, fmt);

		wchar_t fmt3[3*4*32];
		PRINT_STR_BUF(fmt3, 3*4*32, _T("[row0(%s) row1(%s) row2(%s) row3(%s)]"), fmt2, fmt2, fmt2, fmt2);

		wchar_t buf[2048];
		PRINT_STR_BUF(buf, 2048, fmt3,
			m(0, 0), m(0, 1), m(0, 2), m(0, 3),
			m(1, 0), m(1, 1), m(1, 2), m(1, 3),
			m(2, 0), m(2, 1), m(2, 2), m(2, 3),
			m(3, 0), m(3, 1), m(3, 2), m(3, 3));
		return buf;
	}
}


//===================
// TO_STR(MTransform)
//===================
wstring					TO_STR(const MTransform& m, uint rotDecimals, uint decimals)
{
	E_ASSERT(rotDecimals < MAX_DECIMALS);
	E_ASSERT(decimals < MAX_DECIMALS);

	// verify that the transform is ortho.
	E_ASSERT(m.GetRot().IsOrtho());

#if TODO
	// convert rotation to (yaw, pitch, roll).
	float yaw, pitch, roll;
	m.GetRot().ToAngles(yaw, pitch, roll);
	yaw		= ANGLE_TO_PERCENT(yaw);
	pitch	= ANGLE_TO_PERCENT(pitch);
	roll	= ANGLE_TO_PERCENT(roll);

	// get the translation component.
	MVec3 trans(m.GetTrans());

	wchar_t rotFmt[32];
	PRINT_STR_BUF(rotFmt, 32, _T("%%%d.%df%%%%"), ROT_FIELD_SIZE + rotDecimals, rotDecimals);

	wchar_t fmt[32];
	PRINT_STR_BUF(fmt, 32, _T("%%.%df"), decimals);

	wchar_t fmt2[6*32];
	PRINT_STR_BUF(fmt2, 6*32, _T("rot(%s,%s,%s) trans(%s,%s,%s)"),
		rotFmt, rotFmt, rotFmt,
		fmt, fmt, fmt);

	wchar_t buf[2048];
	PRINT_STR_BUF(buf, 2048, fmt2,
		yaw, pitch, roll,
		trans.X(), trans.Y(), trans.Z());
	return buf;
#else
	MMat33 rot(m.GetRot());
	MVec3 trans(m.GetTrans());

	wchar_t fmt[32];
	PRINT_STR_BUF(fmt, 32, _T("%%.%df"), decimals);

	wchar_t fmt2[3*32];
	PRINT_STR_BUF(fmt2, 3*32, _T("%s %s %s"), fmt, fmt, fmt);

	wchar_t fmt3[4*3*32];
	PRINT_STR_BUF(fmt3, 4*3*32, _T("rot[row0(%s) row1(%s) row2(%s)] trans[(%s)]"), fmt2, fmt2, fmt2, fmt2);

	wchar_t buf[2048];
	PRINT_STR_BUF(buf, 2048, fmt3,
		rot(0, 0), rot(0, 1), rot(0, 2),
		rot(1, 0), rot(1, 1), rot(1, 2),
		rot(2, 0), rot(2, 1), rot(2, 2),
		trans.X(), trans.Y(), trans.Z());
	return buf;
#endif
}

