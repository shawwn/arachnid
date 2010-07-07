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
#define	E_ASSERT(x)							assert(x)
#define E_VERIFY(cond, ifFail)				{ assert(##cond); if (!(cond)) { ifFail; } }
#define E_VALIDATE(cond, ctx, msg, ifFail)	{ assert(##cond); if (!(cond)) { E_WARN(##ctx, ##msg); ifFail; } }
#define E_ERROR(ctx, msg)					{ E_WARN(##ctx, ##msg); exit(1); }
#define E_WARN(ctx, msg)					assert(!"warning in "#ctx)

// memory macros
#define E_NEW(ctx, type)					new type
#define E_NEW_ARRAY(ctx, type, count)		new type[count]
#define E_DELETE(ctx, ptr)					{ delete ptr; ptr = NULL; }
#define E_DELETE_ARRAY(ctx, ptr)			{ delete [] ptr; ptr = NULL; }

// misc macros
#define BIT(x)								(1 << (x))
#define E_MIN(x, y)							((x) < (y) ? (x) : (y))
#define E_MAX(x, y)							((x) > (y) ? (x) : (y))
#define E_CLAMP(lo, hi, val)				((val) < (lo) ? (lo) : (val) > (hi) ? (hi) : (val))
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
	if (memA == NULL && memB == NULL)
		return 0;

	if (size == 0)
		return 0;

	// use the standard memcmp for now.
	return memcmp((const void*)memA, (const void*)memB, size);
}


//===================
// MemZero
//===================
inline void					MemZero(byte* buf, uint size)
{
	if (buf == NULL || size == 0)
		return;

	memset(buf, 0, size);
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
	if (ptr == NULL)
		return NULL;

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
	E_VERIFY(src != NULL && size > 0, return);
	memcpy(dst, src, size);
}


//===================
// ArrayZero
//===================
template<class T>
inline void					ArrayZero(const wchar_t* ctx, T* dst, uint count)
{
	if (dst == NULL || count == 0)
		return;

	MemZero((byte*)dst, sizeof(T)*count);
}


//===================
// ArrayResize
//===================
template<class T>
inline void					ArrayResize(const wchar_t* ctx, T*& dst, uint oldSize, uint newSize)
{
	T* result = E_NEW_ARRAY(ctx, T, newSize);

	if (dst != NULL)
	{
		uint keepSize(E_MIN(oldSize, newSize));
		MemCpy(result, dst, sizeof(T) * keepSize);
	}

	E_DELETE_ARRAY(ctx, dst);
	dst = result;
}


//===================
// ArrayCpy
//===================
template<class T>
inline T*					ArrayCpy(const wchar_t* ctx, const T* src, uint count)
{
	if (src == NULL)
		return NULL;

	T* result(E_NEW_ARRAY(ctx, T, count));
	MemCpy(result, src, sizeof(T) * count);
	return result;
}


//===================
// ESwap
//===================
template<class T>
inline void					ESwap(T& a, T& b)
{
	T tmp(a);
	a = b;
	b = tmp;
}
//========================================================================
