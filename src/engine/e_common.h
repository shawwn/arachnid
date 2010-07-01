//========================================================================
//	file:		e_common.h
//	author:		Shawn Presser 
//	date:		6/30/10
//
// (c) 2010 Shawn Presser.  All Rights Reserved.
//========================================================================
#pragma once

//========================================================================
// Headers
//========================================================================
#include "e_api.h"
#include "e_types.h"
#include "e_stl.h"
#include "e_string_utils.h"
#include "e_utils.h"
#include <cassert>
#include <fstream>
//========================================================================

//========================================================================
// Macros
//========================================================================
#define	E_ASSERT(x)					assert(x)
#define E_VERIFY(cond, ifFail)		{ assert(cond); if (!(cond)) { ifFail; } }
//========================================================================