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

enum	ETextEncoding
{
	TE_UTF8,
	TE_UTF8_WITH_BOM,
	TE_UTF16,
	TE_UTF16_LITTLE_ENDIAN,
	TE_UTF32,
	TE_UTF32_LITTLE_ENDIAN
};
//========================================================================

//========================================================================
// String Utils
//========================================================================

// trims whitespace from a string.
extern ENGINE_API bool				StrIsWhitespace(char c);
extern ENGINE_API bool				StrIsWhitespace(wchar_t c);
extern ENGINE_API string			StrTrimLeft(const string& str);
extern ENGINE_API wstring			StrTrimLeft(const wstring& str);
extern ENGINE_API string			StrTrimRight(const string& str);
extern ENGINE_API wstring			StrTrimRight(const wstring& str);
extern ENGINE_API string			StrTrim(const string& str);
extern ENGINE_API wstring			StrTrim(const wstring& str);

// returns whether a string begins/ends with another string.
extern ENGINE_API bool				StrBeginsWith(const string& str, const string& with);
extern ENGINE_API bool				StrBeginsWith(const wstring& str, const wstring& with);
extern ENGINE_API bool				StrEndsWith(const string& str, const string& with);
extern ENGINE_API bool				StrEndsWith(const wstring& str, const wstring& with);

// returns whether 'str' contains 'searchFor'.
extern ENGINE_API bool				StrContains(const string& str, const string& searchFor);
extern ENGINE_API bool				StrContains(const wstring& str, const wstring& searchFor);

// converts a string to lower/upper case.
extern ENGINE_API string			StrLower(const string& str);
extern ENGINE_API wstring			StrLower(const wstring& str);
extern ENGINE_API string			StrUpper(const string& str);
extern ENGINE_API wstring			StrUpper(const wstring& str);

// returns whether two strings are equal, ignoring case.
extern ENGINE_API bool				StrEqualNum(const string& strA, const string& strB, size_t n, size_t off = 0);
extern ENGINE_API bool				StrEqualNum(const wstring& strA, const wstring& strB, size_t n, size_t off = 0);
extern ENGINE_API bool				StrEqualNoCase(const string& strA, const string& strB);
extern ENGINE_API bool				StrEqualNoCase(const wstring& strA, const wstring& strB);
extern ENGINE_API bool				StrEqualNumNoCase(const string& strA, const string& strB, size_t n, size_t off = 0);
extern ENGINE_API bool				StrEqualNumNoCase(const wstring& strA, const wstring& strB, size_t n, size_t off = 0);

// splits a string by delim.
extern ENGINE_API uint				StrSplitBy(strvec& results, const string& str, const string& delim);
extern ENGINE_API uint				StrSplitBy(strvec& results, const wstring& str, const wstring& delim);

// replaces each instance of 'replacing' with 'replaceWith'
extern ENGINE_API string			StrReplace(const string& str, const string& replacing, const string& replaceWith);
extern ENGINE_API wstring			StrReplace(const wstring& str, const wstring& replacing, const wstring& replaceWith);

// replaces a sequence of the same string.  Examples:
//		StrCompact("/path/to/..../file", ".", "..")			=> "/path/to/../file"
//		StrCompact("hello hello there!", "hello ", "ohai ") => "ohai there!"
extern ENGINE_API string			StrCompact(const string& str, const string& sequence, const string& replaceWith, size_t sequenceMinCount = 1);
extern ENGINE_API wstring			StrCompact(const wstring& str, const wstring& sequence, const wstring& replaceWith, size_t sequenceMinCount = 1);

// returns a substring from the beginning until "sequence".  Examples:
//		StrUntil("foo/bar", "/")	=> "foo"
//		StrUntil("foo", "/")		=> "foo"
extern ENGINE_API string			StrUntil(const string& str, const string& sequence);
extern ENGINE_API wstring			StrUntil(const wstring& str, const wstring& sequence);

// text encoding classification.
extern ENGINE_API ETextEncoding		StrClassifyEncoding(uint& outBomSize, const byte* buf, uint bufSize);

// get the length of UTF-8 byte sequence "p[size]".  Does not include any
// following composing characters.
// returns 1 for "".
// returns 1 for an illegal byte sequence (also in incomplete byte seq.).
// returns number > "size" for an incomplete byte sequence.
extern ENGINE_API uint				StrUTF8CharLength(const byte* p, uint size);

// converts a UTF-16 string to UTF-8.
// the input is "utf16str[utf16len]" with "utf16len" in number of UTF-16 words.
// when "oututf8str" is NULL only return the required number of bytes.
// otherwise "oututf8str" must be a buffer of sufficient size.
// return the number of bytes produced.
extern ENGINE_API uint				StrUTF16toUTF8(const wchar_t* utf16str, uint utf16len, byte* oututf8str);

// converts a UTF-8 string to UTF-16.
// "utf8str[utf8len]" is the input.  "utf8len" is in bytes.
// when "oututf16str" is NULL only return the number of UTF-16 words produced,
// otherwise "oututf16str" must be a buffer of sufficient size.
// returns the number of UTF-16 words produced.
extern ENGINE_API uint				StrUTF8toUTF16(const char* utf8str, uint utf8len, wchar_t* oututf16str, int& unconvlen);

// converts a UTF-16 wide-character string to a UTF-8 multi-byte string.
extern ENGINE_API string			WStringToString(const wstring& str);

// converts a UTF-8 multi-byte string to a UTF-16 wide-character string.
extern ENGINE_API wstring			StringToWString(const string& str);

// wide-string helpers.
inline wstring						ToWideString(const char* str)			{ return StringToWString(str); }
inline wstring						ToWideString(const wchar_t* str)		{ return wstring(str); }
inline wstring						ToWideString(const string& str)			{ return StringToWString(str); }
inline const wstring&				ToWideString(const wstring& str)		{ return str; }
inline wstring						ToWideString(char c)					{ return StringToWString(string(1, c)); }
inline wstring						ToWideString(wchar_t c)					{ return wstring(1, c); }
#define _TS(x)						ToWideString(x)
//========================================================================
