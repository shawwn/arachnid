//========================================================================
//	file:		e_utils.cpp
//	author:		Shawn Presser 
//	date:		6/30/10
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
#define TMP_BUFFER_SIZE			(1024 * 1024 * 1024)		// set aside a 1MB temp buffer.
byte		gTmpBuffer[TMP_BUFFER_SIZE];
bool		gTmpBufferInUse;
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


//===================
// TmpAlloc
//===================
byte*			TmpAlloc(uint size)
{
	if (gTmpBufferInUse || (size > TMP_BUFFER_SIZE))
		return MemAlloc(size);

	gTmpBufferInUse = true;
	return gTmpBuffer;
}


//===================
// TmpFree
//===================
void			TmpFree(void* p)
{
	if (p == gTmpBuffer)
		gTmpBufferInUse = false;
	else
		MemFree(p);
}

