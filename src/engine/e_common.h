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
// Headers
//========================================================================
#include "e_config.h"
#include "e_api.h"
#include "e_types.h"
#include "e_stl.h"
#include "e_string_utils.h"
#include "e_exception.h"
#include "e_utils.h"
#include "m_math.h"
#include <cassert>
#include <fstream>
//========================================================================

