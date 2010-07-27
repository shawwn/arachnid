//========================================================================
//	file:		test_containers.cpp
//	author:		Shawn Presser 
//
// (c) 2010 Shawn Presser.  All Rights Reserved.
//========================================================================

//========================================================================
// Headers
//========================================================================
#include "e_common.h"
#include "test_containers.h"
//========================================================================

//===================
// TestContainers::Test
//===================
void			TestContainers::Test()
{
	EVecRawMem<float> vec(2);
	vec.push_back(5.0f);
	vec.push_back(1.0f);
	vec.push_back(3.0f);
	vec.push_back(2.0f);

	vec.reserve(32);

	EVecRawMem<float> vec2(vec);

	for (uint i = 0; i < vec2.size(); ++i)
	{
		float val = vec2[i];
		val = val;
	}
}

