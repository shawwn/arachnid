//========================================================================
//	file:		e_common.h
//	author:		Shawn Presser 
//	date:		6/30/10
//
// (c) 2010 Shawn Presser.  All Rights Reserved.
//========================================================================
#pragma once

//========================================================================
// String Declarations
//========================================================================
#define SNULL				""
#define WSNULL				L""

// internal
#define _T__(x)				L##x

// wide-character string
#define _T(x)				_T__(x)
//========================================================================

//========================================================================
// Compile-Time Configuration
//========================================================================
#include "e_constants.h"
//========================================================================

#if !E_EXCEPTIONS
//========================================================================
// Disable Exceptions
//========================================================================
#define _HAS_EXCEPTIONS		0
#define _TYPEINFO_
#define _INC_TYPEINFO
//========================================================================
#endif

//========================================================================
// Headers
//========================================================================

// std headers.
#include <cassert>
#include <fstream>

// engine headers.
#include "e_api.h"
#include "e_types.h"
#include "e_stl.h"
#include "e_utils.h"
#include "e_string_utils.h"
#include "e_exception.h"
#include "e_string_converters.h"
#include "e_ref.h"
#include "e_vector.h"

// math headers.
#include "m_math.h"

// graphics headers.
#include "gr_config.h"
#include "gr_types.h"
#include "gr_utils.h"
//========================================================================
