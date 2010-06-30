//========================================================================
//	file:		e_common.cpp
//	author:		Shawn Presser 
//	date:		6/30/10
//
// (c) 2010 Shawn Presser.  All Rights Reserved.
//========================================================================

//========================================================================
// Headers
//========================================================================
#include "e_common.h"
//========================================================================

//===================
// EnHashMem
//===================
size_t		EnHashMem(const char* mem, uint size)
{
	const uint FNV_PRIME_32(16777619);
	const uint FNV_OFFSET_BASIS_32(2166136261);

	// use 32-bit prime / offset
	const uint FNV_PRIME(FNV_PRIME_32);
	const uint FNV_OFFSET_BASIS(FNV_OFFSET_BASIS_32);

	// fnv-1a hash.
	size_t h(FNV_OFFSET_BASIS);
	while (size-- != 0)
	{
		h ^= *mem++;
		h *= FNV_PRIME;
	}
	return h;
}

