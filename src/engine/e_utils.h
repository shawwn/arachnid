//========================================================================
//	file:		e_utils.h
//	author:		Shawn Presser 
//	date:		6/30/10
//
// (c) 2010 Shawn Presser.  All Rights Reserved.
//========================================================================
#pragma once

//========================================================================
// Macros
//========================================================================

// debug macros
#define	E_ASSERT(x)						assert(x)
#define E_VERIFY(cond, ifFail)			{ assert(##cond); if (!(cond)) { ifFail; } }
#define E_ERROR(ctx, msg)				{ throw EException(_TS(ctx), _TS(msg)); }
#define E_WARN(ctx, msg)				assert(0 && ##ctx && ##msg)

// memory macros
#define E_NEW(ctx, type)				new type
#define E_NEW_ARRAY(ctx, type, count)	new type[count]
#define E_DELETE(ctx, ptr)				{ delete ptr; ptr = NULL; }
#define E_DELETE_ARRAY(ctx, ptr)		{ delete [] ptr; ptr = NULL; }

// misc macros
#define BIT(x)							(1 << (x))
#define E_MIN(x, y)						((x) < (y) ? (x) : (y))
#define E_MAX(x, y)						((x) > (y) ? (x) : (y))
#define E_CLAMP(lo, hi, val)			((val) < (lo) ? (lo) : (val) > (hi) ? (hi) : (val))
//========================================================================

//========================================================================

//========================================================================
// Utilities
//========================================================================
ENGINE_API size_t			HashMem(const byte* mem, uint size);
//========================================================================

//========================================================================
// Memory Management
//========================================================================

//===================
// MemCmp
//===================
inline int					MemCmp(const byte* memA, const byte* memB, uint size)
{
	// use the standard memcmp for now.
	return memcmp((const void*)memA, (const void*)memB, size);
}


//===================
// MemAlloc
//===================
inline byte*				MemAlloc(uint size)
{
	return (byte*)malloc(size);
}


//===================
// MemFree
//===================
inline void					MemFree(void* p)
{
	free(p);
}


ENGINE_API byte*			TmpAlloc(uint size);
ENGINE_API void				TmpFree(void* p);
//========================================================================

//========================================================================
// Memory Utilities
//========================================================================

//===================
// AlignPtr16
//===================
template< typename T >
inline T*					AlignPtr16(T* ptr)
{
	size_t p((size_t)(void*)ptr);
	p += 0xf;
	p &= ~0xf;
	return (T*)(void*)p;
}


//===================
// MemCpy
//===================
inline void					MemCpy(void* dst, const void* src, uint size)
{
	memcpy(dst, src, size);
}
//========================================================================
