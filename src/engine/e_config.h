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
#define E_DEFAULT_WINDOW_WIDTH		800
#define E_DEFAULT_WINDOW_HEIGHT		600
#define E_DEFAULT_WINDOW_TITLE		_T("Arachnid")

// disable exception handling.
#define E_EXCEPTIONS				0
//========================================================================

#if E_WINDOWS
//========================================================================
// Windows Compile-Time Configuration
//========================================================================
// define our minimum windows version as Windows XP.
#define _WIN32_WINNT		0x0500

// exclude rarely-used stuff from Windows headers.
#define WIN32_LEAN_AND_MEAN
//========================================================================
#endif
