//========================================================================
//	file:		e_config.h
//	author:		Shawn Presser 
//	date:		7/1/10
//
// (c) 2010 Shawn Presser.  All Rights Reserved.
//========================================================================
#pragma once

//========================================================================
// Detect Compile-Time Settings
//========================================================================
#ifdef WIN32
#define E_WINDOWS		1
#endif

#ifdef _DEBUG
#define E_DEBUG			1
#endif
//========================================================================

//========================================================================
// Compile-Time Configuration
//========================================================================
#if E_WINDOWS
#define E_PLATFORM_RENDERER			_T("d3d9")
#else
#define E_PLATFORM_RENDERER			_T("gl2")
#endif

// renderer defaults.
#define E_DEFAULT_WINDOW_WIDTH		1024
#define E_DEFAULT_WINDOW_HEIGHT		768
#define E_DEFAULT_WINDOW_TITLE		_T("Arachnid")
//========================================================================
