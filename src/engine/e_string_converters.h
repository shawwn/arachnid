//========================================================================
//	file:		e_string_converters.h
//	author:		Shawn Presser 
//	date:		7/14/10
//
// (c) 2010 Shawn Presser.  All Rights Reserved.
//========================================================================
#pragma once

//========================================================================
// Declarations
//========================================================================
class MVec3;
class MQuat;
class MMat33;
class MMat44;
class MTransform;
//========================================================================

//========================================================================
// Constants
//========================================================================
#define DEFAULT_DECIMALS					3
#define TRANS_DEFAULT_DECIMALS				3
#define ROT_DEFAULT_DECIMALS				3
#define MAX_DECIMALS						20
//========================================================================

//========================================================================
// String Converters
//========================================================================
extern ENGINE_API const wstring&			TO_STR	(bool b);
extern ENGINE_API const string&				TO_STR_A(bool b);
extern ENGINE_API wstring					TO_STR	(int i);
extern ENGINE_API string					TO_STR_A(int i);
extern ENGINE_API wstring					TO_STR	(uint i);
extern ENGINE_API string					TO_STR_A(uint i);
extern ENGINE_API wstring					TO_STR	(float f, uint decimals = DEFAULT_DECIMALS);
extern ENGINE_API string					TO_STR_A(float f, uint decimals = DEFAULT_DECIMALS);
extern ENGINE_API wstring					TO_STR	(const MVec3& v, uint decimals = TRANS_DEFAULT_DECIMALS);
extern ENGINE_API string					TO_STR_A(const MVec3& v, uint decimals = TRANS_DEFAULT_DECIMALS);
extern ENGINE_API wstring					TO_STR	(const MQuat& q, uint rotDecimals = ROT_DEFAULT_DECIMALS);
extern ENGINE_API string					TO_STR_A(const MQuat& q, uint rotDecimals = ROT_DEFAULT_DECIMALS);
extern ENGINE_API wstring					TO_STR	(const MMat33& m, uint rotDecimals = ROT_DEFAULT_DECIMALS, uint decimals = DEFAULT_DECIMALS);
extern ENGINE_API string					TO_STR_A(const MMat33& m, uint rotDecimals = ROT_DEFAULT_DECIMALS, uint decimals = DEFAULT_DECIMALS);
extern ENGINE_API wstring					TO_STR	(const MMat44& m, uint rotDecimals = ROT_DEFAULT_DECIMALS, uint transDecimals = TRANS_DEFAULT_DECIMALS, uint decimals = DEFAULT_DECIMALS);
extern ENGINE_API string					TO_STR_A(const MMat44& m, uint rotDecimals = ROT_DEFAULT_DECIMALS, uint transDecimals = TRANS_DEFAULT_DECIMALS, uint decimals = DEFAULT_DECIMALS);
extern ENGINE_API wstring					TO_STR	(const MTransform& m, uint rotDecimals = ROT_DEFAULT_DECIMALS, uint transDecimals = TRANS_DEFAULT_DECIMALS);
extern ENGINE_API string					TO_STR_A(const MTransform& m, uint rotDecimals = ROT_DEFAULT_DECIMALS, uint transDecimals = TRANS_DEFAULT_DECIMALS);
//========================================================================
