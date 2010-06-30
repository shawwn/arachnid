//========================================================================
//	file:		e_string_utils.h
//	author:		Shawn Presser 
//	date:		6/30/10
//
// (c) 2010 Shawn Presser.  All Rights Reserved.
//========================================================================
#pragma once

//========================================================================
// Definitions
//========================================================================
#define WHITESPACE					" \t\r\n\v"
//========================================================================

//========================================================================
// String Utils
//========================================================================

// trims whitespace from a string.
extern ENGINE_API bool				StrIsWhitespace(char c);
extern ENGINE_API string			StrTrimLeft(const string& str);
extern ENGINE_API string			StrTrimRight(const string& str);
extern ENGINE_API string			StrTrim(const string& str);

// returns whether a string begins/ends with another string.
extern ENGINE_API bool				StrBeginsWith(const string& str, const string& with);
extern ENGINE_API bool				StrEndsWith(const string& str, const string& with);

// converts a string to lower/upper case.
extern ENGINE_API string			StrLowerString(const string& str);
extern ENGINE_API string			StrUpperString(const string& str);

// returns whether two strings are equal, ignoring case.
extern ENGINE_API bool				StrEqualNum(const string& strA, const string& strB, size_t n, size_t off = 0);
extern ENGINE_API bool				StrEqualNoCase(const string& strA, const string& strB);
extern ENGINE_API bool				StrEqualNumNoCase(const string& strA, const string& strB, size_t n, size_t off = 0);

// splits a string by delim.
extern ENGINE_API string			StrSplitBy(strvec& results, const string& str, const string& delim);
//========================================================================
