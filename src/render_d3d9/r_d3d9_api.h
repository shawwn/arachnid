//========================================================================
//	file:		r_d3d9_api.h
//	author:		Shawn Presser 
//	date:		7/1/10
//
// (c) 2010 Shawn Presser.  All Rights Reserved.
//========================================================================
#pragma once

#ifdef RENDER_D3D9_EXPORTS
#define RENDER_D3D9_API		__declspec(dllexport)
#else
#define RENDER_D3D9_API		__declspec(dllimport)
#endif

#define RENDER_D3D9_EXPORT	extern "C" __declspec(dllexport)

