//========================================================================
//	file:		e_config.h
//	author:		Shawn Presser 
//	date:		7/15/10
//
// (c) 2010 Shawn Presser.  All Rights Reserved.
//========================================================================
#pragma once

// detect compile-time settings.
#include "e_config.h"

//========================================================================
// Special File Paths
//========================================================================
// resolves to e.g. "c:/program files/arachnid/"
#define FILE_PATH_ROOT_DIR		_T('#')

// resolves to e.g. "c:/user/shawn/"
#define FILE_PATH_USER_DIR		_T('~')
//========================================================================

//========================================================================
// Newline Characters
//========================================================================
#if E_WINDOWS
// Windows-style newlines.
#define E_NEWLINE			"\r\n"
#define E_NEWLINE_CHARS		2
#else
// Unix-style newlines.
#define E_NEWLINE			"\n"
#define E_NEWLINE_CHARS		1
#endif
//========================================================================

//========================================================================
// Indent
//========================================================================
#define E_INDENT			"  "
#define E_INDENT_CHARS		2
//========================================================================

//========================================================================
// Compile-Time Settings
//========================================================================
// disable exception handling.
#define E_EXCEPTIONS				0

// renderer defaults.
#define E_DEFAULT_WINDOW_WIDTH		800
#define E_DEFAULT_WINDOW_HEIGHT		600
#define E_DEFAULT_WINDOW_TITLE		_T("Arachnid")
#if E_WINDOWS
#define E_PLATFORM_RENDERER			_T("d3d9")
#else
#define E_PLATFORM_RENDERER			_T("gl2")
#endif
//========================================================================

//========================================================================
// Debug Settings
//========================================================================
#if E_DEBUG
#define E_PRINT_DEBUG_INFO			0
#endif
//========================================================================
