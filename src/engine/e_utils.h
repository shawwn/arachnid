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
#define BIT(x)				(1 << (x))
//========================================================================

//========================================================================
// Utilities
//========================================================================
ENGINE_API size_t			EnHashMem(const char* mem, uint size);


template< typename T >
inline T*					AlignPtr16(T* ptr)
{
	size_t p((size_t)(void*)ptr);
	p += 0xf;
	p &= ~0xf;
	return (T*)(void*)p;
}
//========================================================================

//========================================================================
// Memory Management
//========================================================================
inline int					MemCmp(const byte* memA, const byte* memB, uint size)
{
	// use the standard memcmp for now.
	return memcmp((const void*)memA, (const void*)memB, size);
}


inline byte*				MemAlloc(uint size)
{
	return (byte*)malloc(size);
}


inline void					MemFree(void* p)
{
	free(p);
}


ENGINE_API byte*			TmpAlloc(uint size);
ENGINE_API void				TmpFree(void* p);
//========================================================================