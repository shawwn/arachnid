//========================================================================
//	file:		e_utils.cpp
//	author:		Shawn Presser 
//
// (c) 2010 Shawn Presser.  All Rights Reserved.
//========================================================================

//========================================================================
// Headers
//========================================================================
#include "e_common.h"
#include "e_utils.h"
//========================================================================

//========================================================================
// Globals
//========================================================================
#define TMP_BUFFER_SIZE			(1024 * 1024) // 1MB temp buffer
static byte*					gTmpBuffer;
static bool						gTmpBufferInUse;
//========================================================================

//===================
// EnHashMem
//===================
size_t		HashMem(const byte* mem, uint size)
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


//===================
// TmpAlloc
//===================
byte*			TmpAlloc(uint size)
{
	if (gTmpBufferInUse || size > TMP_BUFFER_SIZE)
		return MemAlloc(size);

	if (gTmpBuffer == NULL)
		gTmpBuffer = MemAlloc(TMP_BUFFER_SIZE);

	gTmpBufferInUse = true;
	return gTmpBuffer;
}


//===================
// TmpFree
//===================
void			TmpFree(void* p)
{
	if (p != gTmpBuffer)
	{
		MemFree(p);
		return;
	}

	E_ASSERT(gTmpBufferInUse);
	gTmpBufferInUse = false;
}

