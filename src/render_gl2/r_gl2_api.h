//========================================================================
//	file:		r_gl2_api.h
//	author:		Shawn Presser 
//	date:		7/1/10
//
// (c) 2010 Shawn Presser.  All Rights Reserved.
//========================================================================
#pragma once

#ifdef RENDER_GL2_EXPORTS
#define RENDER_GL2_API		__declspec(dllexport)
#else
#define RENDER_GL2_API		__declspec(dllimport)
#endif

#define RENDER_GL2_EXPORT	extern "C" __declspec(dllexport)

