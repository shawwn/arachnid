//========================================================================
//	file:		engine_api.h
//	author:		Shawn Presser 
//	date:		6/30/10
//
// (c) 2010 Shawn Presser.  All Rights Reserved.
//========================================================================
#pragma once

#ifdef ENGINE_EXPORTS
#define ENGINE_API		__declspec(dllexport)
#else
#define ENGINE_API		__declspec(dllimport)
#endif
