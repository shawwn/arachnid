//========================================================================
//	file:		e_string_utils.cpp
//	author:		Shawn Presser 
//	date:		6/30/10
//
// (c) 2010 Shawn Presser.  All Rights Reserved.
//========================================================================

//========================================================================
// Headers
//========================================================================
#include "e_common.h"
#include "e_string_utils.h"
#include <ctype.h>
//========================================================================


//===================
// IsWhitespace
//===================
bool			StrIsWhitespace(char c)
{
	const char* p(WHITESPACE);
	char cmp;
	while (cmp = *p++)
	{
		if (c == cmp)
			return true;
	}

	return false;
}


//===================
// StrTrimLeft
//===================
string			StrTrimLeft(const string& str)
{
	size_t pos(0);
	for (; pos < str.size(); ++pos)
	{
		if (!StrIsWhitespace(str[pos]))
			break;
	}
	return str.substr(pos);
}


//===================
// StrTrimRight
//===================
string			StrTrimRight(const string& str)
{
	size_t pos(str.size());
	for (; pos > 0; --pos)
	{
		if (!StrIsWhitespace(str[pos-1]))
			break;
	}
	return str.substr(0, pos);
}


//===================
// StrTrim
//===================
string			StrTrim(const string& str)
{
	return StrTrimLeft(StrTrimRight(str));
}


//===================
// StrBeginsWith
//===================
bool			StrBeginsWith(const string& str, const string& with)
{
	if (StrEqualNum(str, with, with.size()))
		return true;
	return false;
}


//===================
// StrEndsWith
//===================
bool			StrEndsWith(const string& str, const string& with)
{
	if (str.size() < with.size())
		return false;
	if (StrEqualNum(str, with, with.size(), str.size() - with.size()))
		return true;
	return false;
}


//===================
// StrLowerString
//===================
string			StrLowerString(const string& str)
{
	size_t len(str.size());
	string result(len, ' ');
	for (size_t i = 0; i < len; ++i)
		result[i] = tolower(str[i]);
	return result;
}


//===================
// StrUpperString
//===================
string			StrUpperString(const string& str)
{
	size_t len(str.size());
	string result(len, ' ');
	for (size_t i = 0; i < len; ++i)
		result[i] = toupper(str[i]);
	return result;
}


//===================
// StrEqualNum
//===================
bool			StrEqualNum(const string& strA, const string& strB, size_t n, size_t off)
{
	size_t sizeA(strA.size());
	size_t sizeB(strB.size());
	size_t pos(off);
	while (n-- != 0)
	{
		if (pos >= sizeA || pos >= sizeB)
			return false;

		if (strA[pos] != strB[pos])
			return false;
	}

	return true;
}


//===================
// StrEqualNoCase
//===================
bool			StrEqualNoCase(const string& strA, const string& strB)
{
	size_t sizeA(strA.size());
	size_t sizeB(strB.size());
	if (sizeA != sizeB)
		return false;

	for (size_t pos(0); pos < sizeA; ++pos)
		if (tolower(strA[pos]) != tolower(strB[pos]))
			return false;

	return true;
}


//===================
// StrEqualNumNoCase
//===================
bool			StrEqualNumNoCase(const string& strA, const string& strB, size_t n, size_t off)
{
	size_t sizeA(strA.size());
	size_t sizeB(strB.size());
	size_t pos(off);
	while (n-- != 0)
	{
		if (pos >= sizeA || pos >= sizeB)
			return false;

		if (tolower(strA[pos]) != tolower(strB[pos]))
			return false;
	}

	return true;
}


//===================
// SplitBy
//===================
string			SplitBy(strvec& results, const string& str, const string& delim)
{
	results.clear();

	size_t curPos(0);
	size_t findPos(str.find(delim));
	while (findPos != string::npos)
	{
		results.push_back(str.substr(curPos, findPos - curPos));
		curPos = findPos + delim.size();
		findPos = str.find(delim, curPos);
	}
	if (curPos < str.size())
		results.push_back(str.substr(curPos));
}
